#pragma once
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/ConditionalExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class WhileSimpleSyntaxRule : public SimpleSyntaxRule {
private:
    list<Token> conditionTokens;
    list<Token> statementListTokens;
public:
    WhileSimpleSyntaxRule() {
        this->generated = false;
        this->initialized = false;
    }

    vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
    list<Token> consumeTokens(list<Token> tokens) override;
    shared_ptr<ASTNode> constructNode() override;
};
