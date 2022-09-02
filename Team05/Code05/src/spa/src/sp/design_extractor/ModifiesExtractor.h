#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/AST.h>
using namespace std;

class ModifiesExtractor : Extractor<Relationship> {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	vector<Relationship> extract(ASTNode &ast) override;

	vector<Relationship> extractModifies(ASTNode& ast);

	vector<Relationship> recursiveExtract(Entity& LHS, ASTNode& ast);
};