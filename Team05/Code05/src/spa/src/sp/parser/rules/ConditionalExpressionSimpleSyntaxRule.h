#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
using namespace std;

class ConditionalExpressionSimpleSyntaxRule : public SimpleSyntaxRule {
private:
	shared_ptr<SimpleSyntaxRule> operatorRule;
public:
	ConditionalExpressionSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
		this->operatorRule = shared_ptr<SimpleSyntaxRule>();
	}

	void setOperatorRule(shared_ptr<SimpleSyntaxRule> operatorRule) {
		this->operatorRule = operatorRule;
	}

	shared_ptr<SimpleSyntaxRule> getOperatorRule() {
		return this->operatorRule;
	}

	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;
};