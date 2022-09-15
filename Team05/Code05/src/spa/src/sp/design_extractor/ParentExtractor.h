#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Extractor.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/ast/AST.h>
using namespace std;

class ParentExtractor : Extractor<Relationship> {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	virtual vector<Relationship> extract(shared_ptr<ASTNode> ast) = 0;

	/*
		Iterates through the children of a container node extracting the Parent relationship
	*/
	vector<Relationship> extractFromContainer(shared_ptr<ASTNode> containerASTNode, Entity leftHandSide);
};