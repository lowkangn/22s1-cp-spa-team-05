// imported libraries
#include <vector>
#include <memory>


// imported locally
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/design_extractor/ModifiesExtractor.h>

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
	// TODO (Not in milestone 1)
	case ASTNodeType::CALL:
		break;
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
	auto assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
	shared_ptr<ASTNode> leftChild = assignNode->getLeftHandSide();


	// Extracting tokens of the entity which is modified
	Token modifiedToken = leftChild->getToken();
	Entity modifiedEntity = Entity{ EntityType::VARIABLE, leftChild->getLineNumber(), modifiedToken };

	Token lineNumber = Token{ to_string(leftChild->getLineNumber()),TokenType::NAME_OR_KEYWORD };
	Entity leftHandSide = Entity{ EntityType::LINENUMBER, ast->getLineNumber(), lineNumber };

	modifiesRelationships.push_back(Relationship{ leftHandSide, modifiedEntity, RelationshipType::MODIFIES });

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleRead(shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	// Sanity check that there is only one child since this is a read statement;
	auto readNode = dynamic_pointer_cast<ReadASTNode>(ast);
	shared_ptr<ASTNode> child = readNode->getVariableToRead();

	Token lineNumber = Token{ to_string(ast->getLineNumber()),TokenType::NAME_OR_KEYWORD };
	Token childToken = child->getToken();

	Entity leftHandSide = Entity{ EntityType::LINENUMBER, ast->getLineNumber(), lineNumber, };
	Entity rightHandSide = Entity{ EntityType::VARIABLE, child->getLineNumber(), childToken, };

	modifiesRelationships.push_back(Relationship{ leftHandSide, rightHandSide, RelationshipType::MODIFIES });

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	// Get the name ASTNode
	Token procedureName = ast->getToken();

	Entity leftHandSide = Entity{ EntityType::PROCEDURE, ast->getLineNumber(), procedureName };

	vector<Relationship> extractedChildRelationships = recursiveProcedureExtract(leftHandSide, ast);

	return extractedChildRelationships;
}

// TODO in a future iteration
vector<Relationship> ModifiesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> ModifiesExtractor::recursiveProcedureExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		// Get left child
		auto assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
		shared_ptr<ASTNode> leftChild = assignNode->getLeftHandSide();

		Entity childEntity = leftChild->extractEntity();
		Relationship toAdd = Relationship{ leftHandSide, childEntity, RelationshipType::MODIFIES };

		modifiesRelationships.push_back(toAdd);
		break;
	}
	case ASTNodeType::READ:
	{
		// Read should have only one child
		assert(ast->getChildren().size() == 1);
		// Get child
		auto readNode = dynamic_pointer_cast<ReadASTNode>(ast);
		shared_ptr<ASTNode> child = readNode->getVariableToRead();

		Entity childEntity = child->extractEntity();
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