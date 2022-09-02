
// imported libraries
#include<vector>



// imported local files
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/AST.h>
using namespace std;

vector<Entity> EntityExtractor::extract(ASTNode &ast) override {
	vector<Entity> entities = vector<Entity>();

	// Extract out line
	return entities;
}

Entity EntityExtractor::extractLineNumbers(ASTNode& ast) {
	Entity* lineEntity = new Entity(EntityType::LINENUMBER, ast->getLineNumber(), )
}