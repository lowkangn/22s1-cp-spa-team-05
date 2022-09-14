#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/design_extractor/Extractor.h>

using namespace std;

class PatternExtractor : public Extractor<Pattern> {

public:
	PatternExtractor() {};

	/*
		This method is used to extract patterns from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	virtual vector<Pattern> extract(shared_ptr<ASTNode> ast) override;

	Pattern handleIf(shared_ptr<ASTNode> ast);
	Pattern handleAssign(shared_ptr<ASTNode> ast);
	Pattern handleWhile(shared_ptr<ASTNode> ast);

	string recursiveExtractExpression(shared_ptr<ASTNode> ast);
};