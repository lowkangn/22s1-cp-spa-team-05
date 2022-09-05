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
	return entities;
}

vector<Pattern> DesignExtractorManager::extractPatterns(shared_ptr<ASTNode> ast) {
	vector<Pattern> patterns = this->patternExtractor.extract(ast);
	return patterns;
};

void DesignExtractorManager::extractAll() {
	vector<Relationship> extractedRelationships = this->extractRelationships(this->rootNode);
	this->relationships.insert(this->relationships.end(), extractedRelationships.begin(), extractedRelationships.end());

	vector<Entity> extractedEntities = this->extractEntities(this->rootNode);
	this->entities.insert(this->entities.end(), extractedEntities.begin(), extractedEntities.end());
	
	vector<Pattern> extractedPatterns = this->extractPatterns(this->rootNode);
	this->patterns.insert(this->patterns.end(), extractedPatterns.begin(), extractedPatterns.end());
}

void DesignExtractorManager::storeAllRelations(shared_ptr<PKBUpdateHandler> pkb) {
	//pkb.addEntity(this->entities);
	//pkb.addPattern(this->patterns);
	//pkb.addRelationship(this->relationships);
}
