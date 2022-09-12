// imported libraries
#include <vector>
#include <string>


// imported local files
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <memory>

using namespace std;

vector<Relationship> DesignExtractorManager::extractRelationships(shared_ptr<ASTNode> ast) {
	vector<Relationship> relationships = vector<Relationship>();

	// Loops through all the different relationshipExtractor (Modifies, Uses, Parent) and extracts their respective relationships
	for (auto &relationshipExtractor : this->relationshipExtractorList) {
		vector<Relationship> extractedRelations = relationshipExtractor->extract(ast);
		relationships.insert(relationships.end(), extractedRelations.begin(), extractedRelations.end());
	}

	return relationships;
}

vector<Entity> DesignExtractorManager::extractEntities(shared_ptr<ASTNode> ast) {
	vector<Entity> entities = this->entityExtractor.extract(ast);

	// Fix to remove constant entities
	// refer to https://www.techiedelight.com/how-to-filter-a-vector-in-cpp/
	entities.erase(remove_if(entities.begin(), entities.end(), 
		[](Entity entity) {
		return entity.getType() == EntityType::CONSTANT;
		}), entities.end());

	return entities;
}

vector<Pattern> DesignExtractorManager::extractPatterns(shared_ptr<ASTNode> ast) {
	vector<Pattern> patterns = this->patternExtractor.extract(ast);
	return patterns;
};
