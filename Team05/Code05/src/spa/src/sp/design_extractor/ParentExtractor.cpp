#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/design_extractor/ParentExtractor.h>

#include <vector>

using namespace std;

vector<Relationship> ParentExtractor::extract(shared_ptr<ASTNode> ast) {
    vector<Relationship> extractedParentRelationships;

    if (ast->isIfNode()) {
        shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

        Entity ifEntity = ifNode->extractEntity();

        vector<Relationship> thenRelations = this->extractFromContainer(
            ifEntity,
            ifNode->getThenStatements());
        vector<Relationship> elseRelations = this->extractFromContainer(
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

        vector<Relationship> whileParentRelations = this->extractFromContainer(
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

vector<Relationship> ParentExtractor::extractFromContainer(
    Entity leftHandSide, shared_ptr<ASTNode> containerASTNode) {
    // Sanity check
    assert(containerASTNode->isStmtLstNode());

    vector<Relationship> parentRelationships;

    // Find statements and create relationship
    for (shared_ptr<ASTNode> child : containerASTNode->getChildren()) {
        if (child->isStatement()) {
            Entity childEntity = child->extractEntity();
            Relationship parent =
                Relationship::createParentRelationship(leftHandSide, childEntity);
            parentRelationships.push_back(parent);
        }
    }

    return parentRelationships;
}
