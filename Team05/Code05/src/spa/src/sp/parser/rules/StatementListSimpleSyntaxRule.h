#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
using namespace std;

class StatementListSimpleSyntaxRule : public SimpleSyntaxRule {
public:
	StatementListSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}
	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;

	bool isAssignStatement(list<Token> tokens);
	bool isWhileStatement(list<Token> tokens);
	bool isIfStatement(list<Token> tokens);
	bool isPrintStatement(list<Token> tokens);
	bool isReadStatement(list<Token> tokens);
	bool isCallStatement(list<Token> tokens);
};