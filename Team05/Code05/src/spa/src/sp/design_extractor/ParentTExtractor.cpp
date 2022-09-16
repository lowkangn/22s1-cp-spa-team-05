#pragma once
#include <vector>
#include <memory>

#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>

using namespace std;

vector<Relationship> ParentTExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> extractedParentRelationships;

	if (ast->isIfNode()) {
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

		Entity ifEntity = ifNode->extractEntity();

		vector<Relationship> thenRelations = this->recursiveExtractFromContainer(ifNode->getThenStatements(), ifEntity);
		vector<Relationship> elseRelations = this->recursiveExtractFromContainer(ifNode->getElseStatements(), ifEntity);

		extractedParentRelationships.insert(extractedParentRelationships.begin(), thenRelations.begin(), thenRelations.end());
		extractedParentRelationships.insert(extractedParentRelationships.begin(), elseRelations.begin(), elseRelations.end());
	}
	else if (ast->isWhileNode()) {
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);

		Entity whileEntity = whileNode->extractEntity();

		vector<Relationship> whileParentRelations = this->recursiveExtractFromContainer(whileNode->getStmtList(), whileEntity);

		extractedParentRelationships.insert(extractedParentRelationships.begin(), whileParentRelations.begin(), whileParentRelations.end());
	}

	// if there is a container statement with another nested container we need to recursively extract Parent Relationships
	if (ast->hasContainer()) {
		for (shared_ptr<ASTNode> child : ast->getChildren()) {
			vector<Relationship> extractedRelations = this->extract(child);
			extractedParentRelationships.insert(extractedParentRelationships.begin(), extractedRelations.begin(), extractedRelations.end());
		}
	}

	return extractedParentRelationships;
}

vector<Relationship> ParentTExtractor::recursiveExtractFromContainer(shared_ptr<ASTNode> containerASTNode, Entity leftHandSide) {
	// Sanity check
	assert(containerASTNode->isStmtLstNode());

	vector<Relationship> parentRelationships;

	// Find statements and create relationship
	for (shared_ptr<ASTNode> child : containerASTNode->getChildren()) {

		if (child->isIfNode()) {
			// Extract this node as a Relationship
			Relationship parent = Relationship::createParentTRelationship(leftHandSide, child->extractEntity());
			parentRelationships.push_back(parent);

			// Extract deeper nested stmtlsts
			shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(child);
			Entity ifEntity = ifNode->extractEntity();

			vector<Relationship> thenRelations = this->recursiveExtractFromContainer(ifNode->getThenStatements(), leftHandSide);
			vector<Relationship> elseRelations = this->recursiveExtractFromContainer(ifNode->getElseStatements(), leftHandSide);

			parentRelationships.insert(parentRelationships.begin(), thenRelations.begin(), thenRelations.end());
			parentRelationships.insert(parentRelationships.begin(), elseRelations.begin(), elseRelations.end());
		}
		else if (child->isWhileNode()) {
			shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(child);
			Relationship parent = Relationship::createParentTRelationship(leftHandSide, child->extractEntity());

			// Extract this node as a Relationship
			parentRelationships.push_back(parent);

			// Extract deeper nested stmtlsts
			vector<Relationship> whileParentRelations = this->recursiveExtractFromContainer(whileNode->getStmtList(), leftHandSide);

			parentRelationships.insert(parentRelationships.begin(), whileParentRelations.begin(), whileParentRelations.end());
		}
		else if (child->isStatement()) {
			Relationship parent = Relationship::createParentTRelationship(leftHandSide, child->extractEntity());
			parentRelationships.push_back(parent);
		}
	}

	return parentRelationships;
}


