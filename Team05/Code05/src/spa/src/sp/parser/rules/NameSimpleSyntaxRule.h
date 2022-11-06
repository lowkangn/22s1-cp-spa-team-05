#pragma once
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>

#include <list>
#include <memory>
#include <vector>

using namespace std;

class NameSimpleSyntaxRule : public SimpleSyntaxRule {
public:
    NameSimpleSyntaxRule() {
        this->generated = false;
        this->initialized = false;
    }

    vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
    list<Token> consumeTokens(list<Token> tokens) override;
    shared_ptr<ASTNode> constructNode() override;
};
