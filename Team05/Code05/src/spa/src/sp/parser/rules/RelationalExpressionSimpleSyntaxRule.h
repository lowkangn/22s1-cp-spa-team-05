#pragma once
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
#include <sp/parser/rules/RelationalFactorSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>

#include <list>
#include <memory>
#include <vector>


using namespace std;

const int LHS_FACTOR = 0;
const int RHS_FACTOR = 2;
const int RELATIONAL_OPERATOR = 1;


class RelationalExpressionSimpleSyntaxRule : public SimpleSyntaxRule {
public:
    list<Token> lhsTokens;
    list<Token> rhsTokens;
    Token operatorToken = Token::createPlaceholderToken();

    RelationalExpressionSimpleSyntaxRule() {
        this->generated = false;
        this->initialized = false;
    }

    vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
    list<Token> consumeTokens(list<Token> tokens) override;
    shared_ptr<ASTNode> constructNode() override;
};
