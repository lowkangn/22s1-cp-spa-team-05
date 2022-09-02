#pragma once
#include <vector>

#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/AST.h>
using namespace std;

class EntityExtractor : Extractor<Entity> {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	virtual vector<Entity> extract(ASTNode &ast);

	virtual Entity* extractEntity(ASTNode &ast);
};