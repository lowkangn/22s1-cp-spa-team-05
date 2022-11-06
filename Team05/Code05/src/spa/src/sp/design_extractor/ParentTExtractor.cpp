#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/design_extractor/ParentTExtractor.h>

#include <memory>
#include <vector>

using namespace std;

vector<Relationship> ParentTExtractor::extract(shared_ptr<ASTNode> ast) {
    vector<Relationship> extractedParentRelationships;

    if (ast->isIfNode()) {
        shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

        Entity ifEntity = ifNode->extractEntity();

        vector<Relationship> thenRelations = this->recursiveExtractFromContainer(
            ifEntity,
            ifNode->getThenStatements());
        vector<Relationship> elseRelations = this->recursiveExtractFromContainer(
            ifEntity,
            ifNode->getElseStatements());

        extractedParentRelationships.insert(
            extractedParentRelationships.begin(),
            thenRelations.begin(),
            thenRelations.end());
        extractedParentRelationships.insert(
            extractedParentRelationships.begin(),
            elseRelations.begin(),
            elseRelations.end());
    } else if (ast->isWhileNode()) {
        shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);

        Entity whileEntity = whileNode->extractEntity();

        vector<Relationship> whileParentRelations = this->recursiveExtractFromContainer(
            whileEntity,
            whileNode->getStmtList());

        extractedParentRelationships.insert(
            extractedParentRelationships.begin(),
            whileParentRelations.begin(),
            whileParentRelations.end());
    }

    // if there is a container statement with another nested container,
    // we need to recursively extract Parent Relationships
    if (ast->hasContainer()) {
        for (shared_ptr<ASTNode> child : ast->getChildren()) {
            vector<Relationship> extractedRelations = this->extract(child);
            extractedParentRelationships.insert(
                extractedParentRelationships.begin(),
                extractedRelations.begin(),
                extractedRelations.end());
        }
    }

    return extractedParentRelationships;
}

vector<Relationship> ParentTExtractor::recursiveExtractFromContainer(
    Entity leftHandSide, shared_ptr<ASTNode> containerASTNode) {
    // Sanity check
    assert(containerASTNode->isStmtLstNode());

    vector<Relationship> parentRelationships;

    // Find statements and create relationship
    for (shared_ptr<ASTNode> child : containerASTNode->getChildren()) {
        if (child->isIfNode()) {
            // Extract this node as a Relationship
            Entity childEntity = child->extractEntity();
            Relationship parent = Relationship::createParentTRelationship(
                leftHandSide,
                childEntity);
            parentRelationships.push_back(parent);

            // Extract deeper nested stmtlsts
            shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(child);
            Entity ifEntity = ifNode->extractEntity();

            vector<Relationship> thenRelations = this->recursiveExtractFromContainer(
                leftHandSide,
                ifNode->getThenStatements());
            vector<Relationship> elseRelations = this->recursiveExtractFromContainer(
                leftHandSide,
                ifNode->getElseStatements());

            parentRelationships.insert(
                parentRelationships.begin(),
                thenRelations.begin(),
                thenRelations.end());
            parentRelationships.insert(
                parentRelationships.begin(),
                elseRelations.begin(),
                elseRelations.end());
        } else if (child->isWhileNode()) {
            shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(child);
            Entity childEntity = child->extractEntity();
            Relationship parent = Relationship::createParentTRelationship(
                leftHandSide,
                childEntity);

            // Extract this node as a Relationship
            parentRelationships.push_back(parent);

            // Extract deeper nested stmtlsts
            vector<Relationship> whileParentRelations = this->recursiveExtractFromContainer(
                leftHandSide,
                whileNode->getStmtList());

            parentRelationships.insert(
                parentRelationships.begin(),
                whileParentRelations.begin(),
                whileParentRelations.end());
        } else if (child->isStatement()) {
            Entity childEntity = child->extractEntity();
            Relationship parent = Relationship::createParentTRelationship(
                leftHandSide,
                childEntity);
            parentRelationships.push_back(parent);
        }
    }

    return parentRelationships;
}
