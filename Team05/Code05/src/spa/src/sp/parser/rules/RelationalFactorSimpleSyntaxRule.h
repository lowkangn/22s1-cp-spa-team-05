#pragma once
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/ExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>

#include <list>
#include <memory>
#include <vector>

using namespace std;

class RelationalFactorSimpleSyntaxRule : public SimpleSyntaxRule {
public:
    RelationalFactorSimpleSyntaxRule() {
        this->generated = false;
        this->initialized = false;
    }

    vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
    list<Token> consumeTokens(list<Token> tokens) override;
    shared_ptr<ASTNode> constructNode() override;
};
