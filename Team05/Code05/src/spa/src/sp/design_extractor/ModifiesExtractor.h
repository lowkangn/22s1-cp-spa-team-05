#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/AST.h>
#include <memory>
using namespace std;

class ModifiesExtractor : Extractor<Relationship> {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	vector<Relationship> extract(shared_ptr<ASTNode> ast) override;

	/*
		This method extracts a modifies relationship from an Assign or a Read node
	*/
	vector<Relationship> extractModifies(shared_ptr<ASTNode> ast);

	/*
		This method recursively traverses the node adding the Modifies relationship
	*/
	vector<Relationship> recursiveExtract(Entity& LHS, shared_ptr<ASTNode> ast);
};