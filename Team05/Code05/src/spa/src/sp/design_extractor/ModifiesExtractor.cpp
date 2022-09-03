// imported libraries
#include <vector>

// imported locally
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <assert.h>


const int LEFT_CHILD = 0;

vector<Relationship> ModifiesExtractor::extract(shared_ptr<ASTNode> ast) {

	vector<Relationship> modifies = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		vector<Relationship> extractedModifies = this->handleAssign(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	case ASTNodeType::READ:
	{
		vector<Relationship> extractedModifies = this->handleRead(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	case ASTNodeType::PROCEDURE:
	{
		vector<Relationship> extractedModifies = this->handleProcedure(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	default:
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedRelationships = this->extract(child);
			modifies.insert(modifies.end(), extractedRelationships.begin(), extractedRelationships.end());
		}
	}
	return modifies;
}

vector<Relationship> ModifiesExtractor::handleAssign(shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	shared_ptr<ASTNode> left = ast->getChildren()[LEFT_CHILD];

	// Sanity check
	assert(left->getTokens().size() == 1);

	Token leftToken = left->getTokens()[0];
	Entity LHS = Entity{ EntityType::VARIABLE, left->getLineNumber(), leftToken, leftToken.asString() };

	shared_ptr<ASTNode> right = ast->getChildren()[1];

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
		vector<Relationship> extractedChildRelationships = this->recursiveExtract(LHS, right);
		modifiesRelationships.insert(modifiesRelationships.end(), extractedChildRelationships.begin(), extractedChildRelationships.end());
	}
	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleRead(shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	// Sanity check that there is only one child since this is a read statement;
	assert(ast->numChildren() == 1);

	shared_ptr<ASTNode> child = ast->getChildren()[0];

	Token lineNumber = Token{ to_string(ast->getLineNumber()),TokenType::NAME };
	Token childToken = child->getTokens()[0];

	Entity LHS = Entity{ EntityType::LINENUMBER, ast->getLineNumber(), lineNumber, lineNumber.asString() };
	Entity RHS = Entity{ EntityType::VARIABLE, child->getLineNumber(), childToken, childToken.asString() };

	modifiesRelationships.push_back(Relationship{ LHS, RHS, RelationshipType::MODIFIES });

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	// Get the name ASTNode
	shared_ptr<ASTNode> child = ast->getChildren()[LEFT_CHILD];

	Token procedureName = child->getTokens()[0];

	Entity LHS = Entity{ EntityType::PROCEDURE, ast->getLineNumber(), procedureName, procedureName.asString() };

	vector<Relationship> extractedChildRelationships = this->recursiveProcedureExtract(LHS, ast);

	return extractedChildRelationships;
}

vector<Relationship> ModifiesExtractor::recursiveProcedureExtract(Entity& LHS, shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		// Get left child
		shared_ptr<ASTNode> leftChild = ast->getChildren()[LEFT_CHILD];
		assert(leftChild->getType() == ASTNodeType::NAME);
		Token childToken = leftChild->getTokens()[0];
		Entity childEntity = Entity{ EntityType::VARIABLE, leftChild->getLineNumber(), childToken, childToken.asString() };
		Relationship toAdd = Relationship{ LHS, childEntity, RelationshipType::MODIFIES };
		modifiesRelationships.push_back(toAdd);
		break;
	}
	case ASTNodeType::READ:
	{
		// Read should have only one child
		assert(ast->getChildren().size() == 1);
		// Get child
		shared_ptr<ASTNode> child = ast->getChildren()[0];

		assert(child->getType() == ASTNodeType::NAME);

		Token childToken = child->getTokens()[0];
		Entity childEntity = Entity{ EntityType::VARIABLE, child->getLineNumber(), childToken, childToken.asString() };
		Relationship toAdd = Relationship{ LHS, childEntity, RelationshipType::MODIFIES };
		
		modifiesRelationships.push_back(toAdd);
		break;
	}
	default:
	{
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedModifies = recursiveProcedureExtract(LHS, child);
			modifiesRelationships.insert(modifiesRelationships.end(), extractedModifies.begin(), extractedModifies.end());
		}
	}
	}
	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::recursiveExtract(Entity& LHS, shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	if (ast->isTerminal()) {
		Token token = ast->getTokens()[0];
		Entity RHS = Entity{ EntityType::UNDEFINED, -1, Token{"", TokenType::INVALID}, "" };
		if (token.getType() == TokenType::NAME) {
			RHS = Entity{ EntityType::VARIABLE, ast->getLineNumber(), token, token.asString() };
		}
		else {
			RHS = Entity{ EntityType::CONSTANT, ast->getLineNumber(), token, token.asString() };
		}
		modifiesRelationships.push_back(Relationship{ LHS, RHS, RelationshipType::MODIFIES });
	}
	else {
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedModifies = recursiveExtract(LHS, child);
			modifiesRelationships.insert(modifiesRelationships.end(), extractedModifies.begin(), extractedModifies.end());
		}
	}

	return modifiesRelationships;
}