// imported libraries
#include <vector>
#include <string>


// imported local files
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>

vector<Relationship> DesignExtractorManager::extractRelationships(ASTNode &ast) {
	vector<Relationship> relationships = vector<Relationship>();

	for (const auto& relationshipExtractor : this->relationshipExtractorList) {
		vector<Relationship> extractedRelations = relationshipExtractor.extract(ast);
		relationships.insert(relationships.end(), extractedRelations.begin(), extractedRelations.end());
	}

	return relationships;
}

vector<Entity> DesignExtractorManager::extractEntities(ASTNode &ast) {
	vector<Entity> entities = this->entityExtractor.extract(ast);
	return entities;
}

vector<Pattern> DesignExtractorManager::extractPatterns(ASTNode &ast) {
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

void storeAllRelations(string pkb) {
	// Implement call to PKB API to store relations

	// pkb.addEntity(vector<Entity> entities)

	// pkb.addPattern(vector<Pattern> patterns)

	// pkb.addRelationship(vector<Relationship> relationships)

}
