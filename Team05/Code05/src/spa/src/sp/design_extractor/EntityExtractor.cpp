
// imported libraries
#include<vector>



// imported local files
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/AST.h>
using namespace std;

vector<Entity> EntityExtractor::extract(ASTNode &ast) override {
	vector<Entity> entities = vector<Entity>();
	
	Entity* toAdd = this->extractEntity(ast);

	// Add Current node as an entity
	if (toAdd != NULL) {
		entities.push_back(*toAdd);
	}

	// Recursively go down the AST to extract child entities
	if (ast->numChildren() > 0) {
		for (const auto &astChild : ast->getChildren()) {
			vector<Entity> extractedChildEntites = this->extract(astChild);
			entities.insert(entities.end(), extractedChildEntites.begin(), extractedChildEntites.end());
		}
	}

	return entities;
}

Entity* EntityExtractor::extractEntity(ASTNode &ast) {
	ASTNodeType type = ast->getNodeType();
	switch (type) {
		case ASTNodeType::PROCEDURE:
			// First token is "Procedure" second token is the name of the procedure
			Token procedureName = ast->getToken()[1];
			return new Entity(EntityType::PROCEDURE, ast->getLineNumber(), procedureName, procedureName.asString());
		case ASTNodeType::READ:
		case ASTNodeType::PRINT:
		case ASTNodeType::CALL:
		case ASTNodeType::WHILE:
		case ASTNodeType::IF:
			Token stmtName = ast->getToken()[0];
			return new Entity(EntityType::STMT, ast->getLineNumber(), stmtName, stmtName.asString());
		case ASTNodeType::VARIABLE:
			Token variable = ast->getToken()[0];
			return new Entity(EntityType::VARIABLE, ast->getLineNumber(), variable, variable.asString());
		case ASTNodeType::CONSTANT:
			Token constant = ast->getToken()[0];
			return new Entity(EntityType::CONSTANT, ast->getLineNumber(), constant, constant.asString());
		default:
			return NULL;
	}
}