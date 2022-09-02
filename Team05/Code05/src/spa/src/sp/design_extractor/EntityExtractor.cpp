// imported libraries
#include<vector>


// imported local files
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/AST.h>
using namespace std;

vector<Entity> EntityExtractor::extract(ASTNode &ast) {
	vector<Entity> entities = vector<Entity>();
	
	Entity toAdd = this->extractEntity(ast);

	// Add Current node as an entity
	if (toAdd.getType() != EntityType::UNDEFINED) {
		entities.push_back(toAdd);
	}

	// Recursively go down the AST to extract child entities
	if (ast.numChildren() > 0) {
		for (ASTNode &astChild : ast.getChildren()) {
			vector<Entity> extractedChildEntites = this->extract(astChild);
			entities.insert(entities.end(), extractedChildEntites.begin(), extractedChildEntites.end());
		}
	}

	return entities;
}

Entity EntityExtractor::extractEntity(ASTNode &ast) {
	ASTNodeType type = ast.getType();

	switch (type) {
		case ASTNodeType::PROCEDURE:
		case ASTNodeType::CALL:
		{
			// Left Child should contain NAME ASTNode which has the procedure's name
			ASTNode leftChild = ast.getChildren()[0];
			Token procedureName = leftChild.getTokens()[0];
			return Entity{ EntityType::PROCEDURE, ast.getLineNumber(), procedureName, procedureName.asString() };
		}

		case ASTNodeType::ASSIGN:
		case ASTNodeType::READ:
		{
			ASTNode leftChild = ast.getChildren()[0];
			Token variableName = leftChild.getTokens()[0];
			return Entity{ EntityType::VARIABLE, ast.getLineNumber(), variableName, variableName.asString() };
		}

		case ASTNodeType::NAME:
		{
			Token variableName = ast.getTokens()[0];
			return Entity{ EntityType::VARIABLE, ast.getLineNumber(), variableName, variableName.asString() };
		}
		case ASTNodeType::CONSTANT:
		{
			Token constantName = ast.getTokens()[0];
			return Entity{ EntityType::CONSTANT, ast.getLineNumber(), constantName, constantName.asString() };
		}
		default:
			return Entity{ EntityType::UNDEFINED, -1, Token{"INVALID", TokenType::INVALID}, "INVALID"};
	}
}