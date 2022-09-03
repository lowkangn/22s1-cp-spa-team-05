// imported libraries
#include<vector>


// imported local files
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/UnknownASTNodeException.h>
#include <assert.h>
using namespace std;

const int LEFT_CHILD = 0;

vector<Entity> EntityExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Entity> entities = vector<Entity>();
	
	Entity toAdd = this->extractEntity(ast);
	bool includedLocalNode = false;

	// Add Current node as an entity
	if (toAdd.getType() != EntityType::UNDEFINED) {
		entities.push_back(toAdd);
		includedLocalNode = true;
	}

	// Recursively go down the AST to extract child entities
	if (!ast->isTerminal()) {
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		vector<Entity> extractedEntities;
		int startIndex = 0;
		// If an entity was extracted from the current node, we can skip the first child
		// as it has already been extracted
		if (includedLocalNode) {
			startIndex = 1;
		}
		for (int i = startIndex; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			extractedEntities = this->extract(child);
			entities.insert(entities.end(), extractedEntities.begin(), extractedEntities.end());
		}
	}

	return entities;
}

Entity EntityExtractor::extractEntity(shared_ptr<ASTNode> ast) {
	ASTNodeType type = ast->getType();

	switch (type) {
		case ASTNodeType::PROCEDURE:
		case ASTNodeType::CALL:
		{
			// Left Child (0th index) should contain NAME ASTNode which has the procedure's name
			shared_ptr<ASTNode> leftChild = ast->getChildren()[LEFT_CHILD];
			
			Token procedureName = leftChild->getTokenName();
			return Entity{ EntityType::PROCEDURE, ast->getLineNumber(), procedureName, procedureName.getString() };
		}

		case ASTNodeType::ASSIGN:
		case ASTNodeType::READ:
		{
			shared_ptr<ASTNode> leftChild = ast->getChildren()[LEFT_CHILD];
			Token variableName = leftChild->getTokenName();
			return Entity{ EntityType::VARIABLE, ast->getLineNumber(), variableName, variableName.getString() };
		}

		case ASTNodeType::NAME:
		{
			// Sanity check for ensuring that the variable ASTNode only has one token which is its name
			assert(ast->getTokens().size() == 1);
			Token variableName = ast->getTokenName();
			return Entity{ EntityType::VARIABLE, ast->getLineNumber(), variableName, variableName.getString() };
		}
		case ASTNodeType::CONSTANT:
		{
			// Sanity check for ensuring that the constant ASTNode only has one token
			assert(ast->getTokens().size() == 1);
			Token constantName = ast->getTokenName();
			return Entity{ EntityType::CONSTANT, ast->getLineNumber(), constantName, constantName.getString() };
		}
		case ASTNodeType::STMTLIST:
		case ASTNodeType::PRINT:
		case ASTNodeType::IF:
		case ASTNodeType::OPERATOR:
		case ASTNodeType::WHILE:
		case ASTNodeType::EXPRESSION:
			// Place holder as StmtLst are not Entities by themselves
			return Entity{ EntityType::UNDEFINED, ast->getLineNumber(), Token{"", TokenType::INVALID}, "" };
		default:
			throw UnknownASTNodeTypeException();
	}
}