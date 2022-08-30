#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/AST.h>
using namespace std;

class RelationshipExtractor {

public: 
	/*
		This method is used to extract relationships from a provided abstract syntax tree. It is meant to be 
		overriden for each specific relationship.
	*/
	virtual vector<Relationship> extract(AST ast);
};