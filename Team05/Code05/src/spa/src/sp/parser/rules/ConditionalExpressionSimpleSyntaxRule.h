#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
using namespace std;

class ConditionalExpressionSimpleSyntaxRule : public SimpleSyntaxRule {
private:
	bool notOperatorUsed;
	bool twoConditionals;
public:
	ConditionalExpressionSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
		this->notOperatorUsed = false;
		this->twoConditionals = false;
	}

	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;
	list<Token> parseCondition(list<Token> &tokens);
};