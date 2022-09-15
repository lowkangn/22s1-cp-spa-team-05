#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>

using namespace std;

vector<Relationship> ParentExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> extractedParentRelationships;

	if (ast->isIfNode()) {
		shared_ptr<IfASTNode> ifNode = dynamic_cast<IfASTNode>(ast);

		Entity ifEntity = ifNode->extractEntity();
		
		vector<Relationship> thenRelations = this->extractFromContainer(ifNode->getThenStatements(), ifEntity);
		vector<Relationship> elseRelations = this->extractFromContainer(ifNode->getElseStatements(), ifEntity);

		extractedParentRelationships.insert(extractedParentRelationships.begin(), thenRelations.begin(), thenRelations.end());
		extractedParentRelationships.insert(extractedParentRelationships.begin(), elseRelations.begin(), elseRelations.end());
	}
	else if (ast->isWhileNode()) {
		shared_ptr<WhileASTNode> whileNode = dynamic_cast<WhileASTNode>(ast);

		Entity whileEntity = whileNode->extractEntity();

		vector<Relationship> whileParentRelations = this->extractFromContainer(whileNode->getStmtList(), whileEntity);

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

vector<Relationship> ParentExtractor::extractFromContainer(shared_ptr<ASTNode> containerASTNode, Entity leftHandSide) {
	vector<Relationship> parentRelationships;

	for (shared_ptr<ASTNode> child : containerASTNode->getChildren()) {
		ASTNodeType astType = child->getType();

		switch (astType) {
		case ASTNodeType::READ:
		case ASTNodeType::ASSIGN:
		case ASTNodeType::IF:
		case ASTNodeType::WHILE:
		case ASTNodeType::CALL:
			Relationship parent = Relationship(leftHandSide, child->extractEntity(), RelationshipType::PARENT);
			parentRelationships.push_back(parent);
		}
	}

	return parentRelationships;
}


