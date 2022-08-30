#pragma once
#include <vector>
#include <sp/dataclasses/Pattern.h>
#include <sp/dataclasses/AST.h>
using namespace std;

class PatternExtractor {

public:
	/*
		This method is used to extract patterns from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	virtual vector<Pattern> extract(AST ast) = 0;
};