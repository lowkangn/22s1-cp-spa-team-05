#pragma once
#include <vector>
#include <sp/dataclasses/Pattern.h>
#include <sp/dataclasses/AST.h>
#include <sp/design_extractor/Extractor.h>
using namespace std;

class PatternExtractor : Extractor<Pattern> {

public:
	/*
		This method is used to extract patterns from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	virtual vector<Pattern> extract(shared_ptr<ASTNode> ast);
};