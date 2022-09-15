#pragma once
#include <list>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/RelationalExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
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