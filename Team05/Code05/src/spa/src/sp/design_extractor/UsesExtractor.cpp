#include <vector>
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <memory>
#include <assert.h>

vector<Relationship> UsesExtractor::extract(shared_ptr<ASTNode> ast) {

	vector<Relationship> uses = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::PROCEDURE:
	{
		vector<Relationship> extractedUses = this->handleProcedure(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::ASSIGN:
	{
		vector<Relationship> extractedUses = this->handleAssign(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::PRINT:
	{
		vector<Relationship> extractedUses = this->handlePrint(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::WHILE:
	{
		vector<Relationship> extractedUses = this->handleWhile(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::IF:
	{
		vector<Relationship> extractedUses = this->handleIf(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::CALL:
	{
		vector<Relationship> extractedUses = this->handleCall(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	default:
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedRelationships = this->extract(child);
			uses.insert(uses.end(), extractedRelationships.begin(), extractedRelationships.end());
		}
	}
	return uses;
}

// TODO in a future iteration
vector<Relationship> UsesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> UsesExtractor::handleAssign(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> UsesExtractor::handlePrint(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> UsesExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> UsesExtractor::handleIf(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

// TODO in a future iteration
vector<Relationship> UsesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}