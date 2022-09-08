// imported libraries
#include<vector>


// imported local files
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/design_extractor/UnknownASTNodeException.h>
#include <string>
#include <assert.h>
using namespace std;

vector<Entity> EntityExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Entity> entities = vector<Entity>();
	
	Entity toAdd = ast->extractEntity();

	// Add Current node as an entity
	if (toAdd.getType() != EntityType::UNDEFINED) {
		entities.push_back(toAdd);
	}

	// Recursively go down the AST to extract child entities
	if (!ast->isTerminal()) {
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		vector<Entity> extractedEntities;
		// If an entity was extracted from the current node, we can skip the first child
		// as it has already been extracted
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			extractedEntities = this->extract(child);
			entities.insert(entities.end(), extractedEntities.begin(), extractedEntities.end());
		}
	}
	return entities;
}