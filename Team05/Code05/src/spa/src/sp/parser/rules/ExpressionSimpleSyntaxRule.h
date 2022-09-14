#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
using namespace std;

class ExpressionSimpleSyntaxRule : public SimpleSyntaxRule {
private:
public:
	list<Token> lhsTokens;
	bool lhsBrackets = false;
	Token operatorToken = Token::getPlaceHolderToken(); // Placeholder has to be replaced during initialization
	list<Token> rhsTokens;
	bool rhsBrackets = false;


	ExpressionSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}
	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;
	list<Token> extractBracketTokens(list<Token> &tokens);
};