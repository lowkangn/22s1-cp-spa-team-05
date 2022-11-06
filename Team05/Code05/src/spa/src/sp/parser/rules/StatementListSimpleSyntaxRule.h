#pragma once
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/rules/CallSimpleSyntaxRule.h>
#include <sp/parser/rules/IfSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/PrintSimpleSyntaxRule.h>
#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/WhileSimpleSyntaxRule.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class StatementListSimpleSyntaxRule : public SimpleSyntaxRule {
public:
    StatementListSimpleSyntaxRule() {
        this->generated = false;
        this->initialized = false;
    }

    vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
    list<Token> consumeTokens(list<Token> tokens) override;
    shared_ptr<ASTNode> constructNode() override;

    bool isAssignStatement(list<Token> tokens);
    bool isWhileStatement(list<Token> tokens);
    bool isIfStatement(list<Token> tokens);
    bool isPrintStatement(list<Token> tokens);
    bool isReadStatement(list<Token> tokens);
    bool isCallStatement(list<Token> tokens);
};
