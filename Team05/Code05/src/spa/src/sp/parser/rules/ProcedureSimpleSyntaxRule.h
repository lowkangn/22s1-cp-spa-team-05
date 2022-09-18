#pragma once
#include <list>
#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/IfSimpleSyntaxRule.h>
#include <sp/parser/rules/WhileSimpleSyntaxRule.h>
#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

using namespace std;

class ProcedureSimpleSyntaxRule : public SimpleSyntaxRule {
public:

	ProcedureSimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}
	vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() override;
	list<Token> consumeTokens(list<Token> tokens) override;
	shared_ptr<ASTNode> constructNode() override;
};