#pragma once
#include <vector>
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>

#include <memory>
#include <list>
using namespace std;

class ProgramSimpleSyntaxRule : public SimpleSyntaxRule {
public:
	ProgramSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}
	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;

	/*
		Wrapper for setting the line numbers in a AST
	*/
	int setASTLineNumbers(shared_ptr<ASTNode> root, int lineNumber);
	
	/*
		This function sets the line numbers of statement lists
	*/
	int iterateThroughStatementInStatementList(shared_ptr<ASTNode> root, int lineNumber);

	/*
		This function recursively sets linenumber for nodes depending on their type
	*/
	void recursiveSetLineNumber(shared_ptr<ASTNode> root, int lineNumber);

	/*
		This function recursively sets linenumber for nodes and its children
	*/
	void recursiveSetStatementNumber(shared_ptr<ASTNode> root, int lineNumber);
};