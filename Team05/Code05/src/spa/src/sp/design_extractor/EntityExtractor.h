#pragma once
#include <vector>
#include <design_objects/Entity.h>
#include <dataclasses/AST.h>
using namespace std;

class EntityExtractor {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	virtual vector<Entity> extract(AST ast)=0;
};