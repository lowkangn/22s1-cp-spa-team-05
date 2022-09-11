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

	// Only add Entities, Program and Stmtlist containers by themselves (nor do they contain line numbers) are not entities
	if (!(toAdd.getType() == EntityType::PROGRAM || toAdd.getType() == EntityType::STMTLIST)) {
		entities.push_back(toAdd);
	}

	// Recursively go down the AST to extract child entities
	if (!ast->isTerminal()) {
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		vector<Entity> extractedEntities;

		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			extractedEntities = this->extract(child);
			// Appending a vector to a vector https://stackoverflow.com/questions/2551775/appending-a-vector-to-a-vector
			entities.insert(entities.end(), extractedEntities.begin(), extractedEntities.end());
		}
	}
	return entities;
}