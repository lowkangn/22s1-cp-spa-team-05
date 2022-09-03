// imported libraries
#include <vector>

// imported locally
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/ModifiesExtractor.h>



vector<Relationship> ModifiesExtractor::extract(ASTNode &ast) {

	vector<Relationship> modifies = vector<Relationship>();

	if (ast.getType() == ASTNodeType::ASSIGN) {
		vector<Relationship> extractedModifies = this->extractModifies(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
	}
	else {
		vector<ASTNode*> children = ast.getChildren();
		for (int i = 0; i < children.size(); i++) {
			ASTNode* child = children[i];
			vector<Relationship> extractedRelationships = this->extract(*child);
			modifies.insert(modifies.end(), extractedRelationships.begin(), extractedRelationships.end());
		}
	}

	return modifies;
}

vector<Relationship> ModifiesExtractor::extractModifies(ASTNode& ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	ASTNode* left = ast.getChildren()[0];
	Token leftToken = left->getTokens()[0];
	Entity LHS = Entity{ EntityType::VARIABLE, left->getLineNumber(), leftToken, leftToken.asString() };

	ASTNode* right = ast.getChildren()[1];

	if (right->isTerminal()) {
		Token rightToken = right->getTokens()[0];
		Entity RHS = Entity{ EntityType::UNDEFINED, -1, Token{"", TokenType::INVALID}, "" };
		if (rightToken.getType() == TokenType::NAME) {
			RHS = Entity{ EntityType::VARIABLE, right->getLineNumber(), rightToken, rightToken.asString() };
		}
		else {
			RHS = Entity{ EntityType::CONSTANT, right->getLineNumber(), rightToken, rightToken.asString() };
		}
		modifiesRelationships.push_back(Relationship{LHS, RHS, RelationshipType::MODIFIES});
	}
	else {
		vector<Relationship> extractedChildRelationships = this->recursiveExtract(LHS, *right);
		modifiesRelationships.insert(modifiesRelationships.end(), extractedChildRelationships.begin(), extractedChildRelationships.end());
	}
	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::recursiveExtract(Entity& LHS, ASTNode& ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	if (ast.isTerminal()) {
		Token rightToken = ast.getTokens()[0];
		Entity RHS = Entity{ EntityType::UNDEFINED, -1, Token{"", TokenType::INVALID}, "" };
		if (rightToken.getType() == TokenType::NAME) {
			RHS = Entity{ EntityType::VARIABLE, ast.getLineNumber(), rightToken, rightToken.asString() };
		}
		else {
			RHS = Entity{ EntityType::CONSTANT, ast.getLineNumber(), rightToken, rightToken.asString() };
		}
		modifiesRelationships.push_back(Relationship{ LHS, RHS, RelationshipType::MODIFIES });
	}
	else {
		vector<ASTNode*> children = ast.getChildren();
		for (int i = 0; i < children.size(); i++) {
			ASTNode* child = children[i];
			vector<Relationship> extractedModifies = recursiveExtract(LHS, *child);
			modifiesRelationships.insert(modifiesRelationships.end(), extractedModifies.begin(), extractedModifies.end());
		}
	}

	return modifiesRelationships;
}