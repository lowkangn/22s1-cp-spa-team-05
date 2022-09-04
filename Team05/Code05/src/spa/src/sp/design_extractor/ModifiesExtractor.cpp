// imported libraries
#include <vector>

// imported locally
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <memory>
#include <assert.h>




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

	// This is the Left hand side of the assign relation
	shared_ptr<ASTNode> left = ast->getChildren()[ASTNode::LEFT_CHILD];


	// Extracting tokens to create a Entity to be used in the Relationship class
	Token leftToken = left->getNameToken();
	Entity leftHandSide = Entity{ EntityType::VARIABLE, left->getLineNumber(), leftToken, leftToken.getString() };

	// The rest of the expression
	shared_ptr<ASTNode> right = ast->getChildren()[ASTNode::RIGHT_CHILD];

	// Recursively check the right hand side for Entities to add to the Modifies relationship
	if (right->isTerminal()) {
		Token rightToken = right->getNameToken();
		Entity RHS = Entity{ EntityType::UNDEFINED, -1, Token{"", TokenType::INVALID}, "" };
		if (rightToken.getType() == TokenType::NAME_OR_KEYWORD) {
			RHS = Entity{ EntityType::VARIABLE, right->getLineNumber(), rightToken, rightToken.getString() };
		}
		else {
			RHS = Entity{ EntityType::CONSTANT, right->getLineNumber(), rightToken, rightToken.getString() };
		}
		modifiesRelationships.push_back(Relationship{ leftHandSide, RHS, RelationshipType::MODIFIES});
	}
	else {
		vector<Relationship> extractedChildRelationships = this->recursiveExtract(leftHandSide, right);
		modifiesRelationships.insert(modifiesRelationships.end(), extractedChildRelationships.begin(), extractedChildRelationships.end());
	}
	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleRead(shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	// Sanity check that there is only one child since this is a read statement;
	assert(ast->numChildren() == 1);
	shared_ptr<ASTNode> child = ast->getChildren()[0];

	Token lineNumber = Token{ to_string(ast->getLineNumber()),TokenType::NAME_OR_KEYWORD };
	Token childToken = child->getNameToken();

	Entity leftHandSide = Entity{ EntityType::LINENUMBER, ast->getLineNumber(), lineNumber, lineNumber.getString() };
	Entity rightHandSide = Entity{ EntityType::VARIABLE, child->getLineNumber(), childToken, childToken.getString() };

	modifiesRelationships.push_back(Relationship{ leftHandSide, rightHandSide, RelationshipType::MODIFIES });

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	// Get the name ASTNode
	shared_ptr<ASTNode> child = ast->getChildren()[ASTNode::LEFT_CHILD];

	Token procedureName = child->getNameToken();

	Entity leftHandSide = Entity{ EntityType::PROCEDURE, ast->getLineNumber(), procedureName, procedureName.getString() };

	vector<Relationship> extractedChildRelationships = this->recursiveProcedureExtract(leftHandSide, ast);

	return extractedChildRelationships;
}

// TODO in a future iteration
vector<Relationship> ModifiesExtractor::handleContainer(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

// TODO in a future iteration
vector<Relationship> ModifiesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

// TODO Can maybe improve efficiency
vector<Relationship> ModifiesExtractor::recursiveProcedureExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		// Get left child
		shared_ptr<ASTNode> leftChild = ast->getChildren()[ASTNode::LEFT_CHILD];
		
		Token childToken = leftChild->getNameToken();
		Entity childEntity = Entity{ EntityType::VARIABLE, leftChild->getLineNumber(), childToken, childToken.getString() };
		Relationship toAdd = Relationship{ leftHandSide, childEntity, RelationshipType::MODIFIES };
		
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

		Token childToken = child->getNameToken();
		Entity childEntity = Entity{ EntityType::VARIABLE, child->getLineNumber(), childToken, childToken.getString() };
		Relationship toAdd = Relationship{ leftHandSide, childEntity, RelationshipType::MODIFIES };
		
		modifiesRelationships.push_back(toAdd);
		break;
	}
	default:
	{
		// Iterate through child nodes calling this function recursively
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedModifies = recursiveProcedureExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), extractedModifies.begin(), extractedModifies.end());
		}
	}
	}
	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::recursiveExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	if (ast->isTerminal()) {
		// Sanity check 
		assert(ast->getTokens().size() == 1);

		Token token = ast->getTokens()[0];
		Entity rightHandSide = Entity{ EntityType::UNDEFINED, -1, Token{"", TokenType::INVALID}, "" };
		if (token.getType() == TokenType::NAME_OR_KEYWORD) {
			rightHandSide = Entity{ EntityType::VARIABLE, ast->getLineNumber(), token, token.getString() };
		}
		else {
			rightHandSide = Entity{ EntityType::CONSTANT, ast->getLineNumber(), token, token.getString() };
		}
		modifiesRelationships.push_back(Relationship{ leftHandSide, rightHandSide, RelationshipType::MODIFIES });
	}
	else {
		// Iterate through child nodes calling this function recursively
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedModifies = recursiveExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), extractedModifies.begin(), extractedModifies.end());
		}
	}

	return modifiesRelationships;
}