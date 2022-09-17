#pragma once
#include <vector>
#include <memory>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
using namespace std;

class ParentTExtractor : Extractor<Relationship> {

public:
	/*
		This method is used to extract realtionships from a provided abstract syntax tree. It is meant to be
		overriden for each specific relationship type.
	*/
	virtual vector<Relationship> extract(shared_ptr<ASTNode> ast) override;

	/*
		Iterates through the children of a container node extracting the Parent relationships
	*/
	vector<Relationship> recursiveExtractFromContainer(Entity leftHandSide, shared_ptr<ASTNode> containerASTNode);
};