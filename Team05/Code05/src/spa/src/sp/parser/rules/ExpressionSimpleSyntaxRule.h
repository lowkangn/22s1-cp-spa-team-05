#pragma once
#include <list>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

using namespace std;

class ExpressionSimpleSyntaxRule : public SimpleSyntaxRule {
private:
public:
	list<Token> lhsTokens;
	bool lhsIsExpression = false;
	bool rhsIsExpression = false;
	Token operatorToken = Token::getPlaceHolderToken(); // Placeholder has to be replaced during initialization
	list<Token> rhsTokens;
	bool isSingleExpression = false;

	ExpressionSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}
	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;
	list<Token> extractBracketTokens(list<Token> &tokens);
};