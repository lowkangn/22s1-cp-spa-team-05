#pragma once
#include <vector>
#include <memory>
#include <sp/design_extractor/Extractor.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/NextExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/ast/AST.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <sp/design_extractor/Extractor.h>


using namespace std;

class DesignExtractorManager {
private:
	shared_ptr<ASTNode> rootNode;

	vector<shared_ptr<Extractor<Relationship>>> relationshipExtractorList;
	EntityExtractor entityExtractor;
	PatternExtractor patternExtractor;
	NextExtractor nextExtractor;


	vector<Relationship> relationships;
	vector<Entity> entities;
	vector<Pattern> patterns;


public:
	/*
		Constructor which takes in all the extractors
	*/
	DesignExtractorManager(EntityExtractor entityExtractor, PatternExtractor patternExtractor, NextExtractor nextExtractor, vector<shared_ptr<Extractor<Relationship>>> relationshipExtractorList) {
		this->entityExtractor = entityExtractor;
		this->patternExtractor = patternExtractor;
		this->nextExtractor = nextExtractor;
		this->relationshipExtractorList = relationshipExtractorList;
	}

	/*
		Sets the root node
	*/
	void setRootNode(shared_ptr<ASTNode> ast) {
		this->rootNode = ast;
	}

	/*
		Extracts all the Relationships from the AST
	*/
	vector<Relationship> extractRelationships(shared_ptr<ASTNode> ast);

	/*
		Extracts all the Relationships from the CFG
	*/
	vector<Relationship> extractCFGRelationships(vector<shared_ptr<CFGNode>> cfgs);

	/*
		Extracts all the Entities from the AST
	*/
	vector<Entity> extractEntities(shared_ptr<ASTNode> ast);

	/*
		Extracts all the patterns from the AST
	*/
	vector<Pattern> extractPatterns(shared_ptr<ASTNode> ast);
};