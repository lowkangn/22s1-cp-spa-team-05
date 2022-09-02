#pragma once
#include <vector>
#include <sp/design_extractor/RelationshipExtractor.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/Extractor.h>


using namespace std;

class DesignExtractorManager {
private:
	vector<ASTNode> rootNode;
	
	vector<Extractor> relationshipExtractorList;
	EntityExtractor entityExtractor;
	PatternExtractor patternExtractor;


	vector<Relationship> relationships;
	vector<Entity> entities;
	vector<Pattern> patterns;

public:
	/*
		Constrcutor which takes in all the extractors
	*/
	DesignExtractorManager(EntityExtractor entityExtractor, PatternExtractor patternExtractor, vector<Extractor> relationshipExtractorList) {
		this->entityExtractor = entityExtractor;
		this->patternExtractor = entityExtractor;
		this->relationshipExtractorList = relationshipExtractorList;
	}

	/*
		Traverses the entire AST Tree and extracts all the relations
	*/
	void extractAll();

	/*
		Stores all the Relationships, Entities, Patterns into the PKB
	*/
	void storeAllRelations();

	/*
		Extracts all the Relationships from the AST
	*/
	vector<Relationship> extractRelationships(ASTNode &ast);

	/*
		Extracts all the Entities from the AST
	*/
	vector<Entity> extractEntities(ASTNode &ast);

	/*
		Extracts all the patterns from the AST
	*/
	vector<Pattern> extractPatterns(ASTNode &ast);
};