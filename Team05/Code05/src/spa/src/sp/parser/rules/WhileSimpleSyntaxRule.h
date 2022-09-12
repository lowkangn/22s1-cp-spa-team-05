#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
using namespace std;

class WhileSimpleSyntaxRule : public SimpleSyntaxRule {

public:
	WhileSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}
	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;
};