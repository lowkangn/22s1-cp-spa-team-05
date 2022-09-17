#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/design_extractor/Extractor.h>

using namespace std;

class FollowsExtractor : Extractor<Relationship> {

public:
	/*
		This method is used to extract relationships from a provided abstract syntax tree. It is meant to be
		overriden for each specific relationship type.
	*/
	virtual vector<Relationship> extract(shared_ptr<ASTNode> ast) = 0;

	/*
		Iterates through the children of a container node extracting the Follows relationships
	*/
	vector<Relationship> extractFromContainer(Entity leftHandSide, shared_ptr<ASTNode> containerASTNode);
};
