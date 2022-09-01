#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
using namespace std;

class ProcedureSimpleSyntaxRule : SimpleSyntaxRule {
public:
	vector<SimpleSyntaxRule> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
};