#pragma once
#include <vector>
#include <stack>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/design_extractor/Extractor.h>

using namespace std;

class PatternExtractor : public Extractor<Pattern> {

public:
	PatternExtractor() {};

	/*
		This method is used to extract patterns from a provided abstract syntax tree.
	*/
	virtual vector<Pattern> extract(shared_ptr<ASTNode> ast) override;

	vector<Pattern> handleIf(shared_ptr<ASTNode> ast);
	vector<Pattern> handleAssign(shared_ptr<ASTNode> ast);
	vector<Pattern> handleWhile(shared_ptr<ASTNode> ast);
	vector<Pattern> handleContainer(shared_ptr<ASTNode> ast);
	string handleCondition(shared_ptr<ASTNode> ast);


	string recursiveExtractExpression(shared_ptr<ASTNode> ast);

};