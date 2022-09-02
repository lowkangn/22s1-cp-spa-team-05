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
	ASTNode constructNode() override;
};