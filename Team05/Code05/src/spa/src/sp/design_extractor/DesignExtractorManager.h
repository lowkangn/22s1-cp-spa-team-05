#pragma once
#include <vector>
#include <sp/design_extractor/RelationshipExtractor.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/AST.h>


using namespace std;

class DesignExtractorManager {
private:
	vector<RelationshipExtractor> relationshipExtractors;
	vector<EntityExtractor> entityExtractor;
	vector<PatternExtractor> patternExtractor;

public: 
	virtual vector<Relationship> extractRelationships(ASTNode ast) = 0;
	virtual vector<Entity> extractEntities(ASTNode ast) = 0;
	virtual vector<Pattern> extractPatterns(ASTNode ast) = 0;
};