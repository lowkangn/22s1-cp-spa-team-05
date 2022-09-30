#include "catch.hpp"

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>


#include <sp/dataclasses/tokens/Token.h>
#include <list>
#include <memory>

#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>


TEST_CASE("ProcedureSimpleSyntaxRule::consumeTokens") {

	auto test = [](SimpleSyntaxRule& rule, list<Token> tokens, list<Token> expectedTokens) {

		// ----- when -----
		list<Token> remainingTokens = rule.consumeTokens(tokens);

		// ----- then -----
		bool sameSize = remainingTokens.size() == expectedTokens.size();
		REQUIRE(sameSize); // same size
		if (sameSize) {
			while (!remainingTokens.empty()) {
				Token t1 = remainingTokens.front();
				Token t2 = expectedTokens.front();
				REQUIRE(t1 == t2);
				remainingTokens.pop_front();
				expectedTokens.pop_front();
			}
		}
	};

	auto testThrowsException = [](SimpleSyntaxRule& rule, list<Token> tokens) {

		// ----- when & then -----
		REQUIRE_THROWS(rule.consumeTokens(tokens));
	};

	SECTION("ProcedureSimpleSyntaxRule: Consumes exactly correct tokens, using example with just an assign") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		list<Token> expectedTokens = {};
		test(ProcedureSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing procedure keyword") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing name") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing {") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing }") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}

	SECTION("ProcedureSimpleSyntaxRule: empty procedure") {
		list<Token> tokens = {
		Token::createProcedureToken(),
			Token::createNameOrKeywordToken("procedureName"),
			Token::createOpenCurlyBracketToken(),
			Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("ProcedureSimpleSyntaxRule::generateChildRules") {

	auto test = [](SimpleSyntaxRule& rule, list<Token> tokensToConsume, vector<shared_ptr<SimpleSyntaxRule>> expectedChildren) {

		// ----- given -----
		// consume tokens and initialize rule
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);

		// ----- when ------
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// ----- then -----
		REQUIRE(childRules.size() == expectedChildren.size()); // same size
		for (int i = 0; i < childRules.size(); i++) {
			auto rule = childRules.at(i);
			auto otherRule = expectedChildren.at(i);
			REQUIRE((*rule).equals(*otherRule));
		}
	};

	SECTION("ProcedureSimpleSyntaxRule: nested assignment statement") {
		// tokens, an assign statement in a procedure
		list<Token> tokensToConsume = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> procedureName = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> procedureNameTokens = { Token::createNameOrKeywordToken("procedureName") };
		procedureName->consumeTokens(procedureNameTokens);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> statementList = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> statementListTokens = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		statementList->consumeTokens(statementListTokens);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				procedureName,
				statementList
		};
		test(ProcedureSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("ProcedureSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("ProcedureSimpleSyntaxRule : constructNode") {
		/*
			procedure procedureName {
				soomevariable = 1;
			}
		*/

		Token procedureToken = Token::createProcedureToken();
		Token procedureName = Token::createNameOrKeywordToken("procedureName");
		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token equalsToken = Token::createEqualsToken();
		Token constantToken = Token::createIntegerToken("1");

		list<Token> tokensToConsume = {
				procedureToken,
				procedureName,
				Token::createOpenCurlyBracketToken(),
				variable,
				equalsToken,
				constantToken,
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// Create rule
		ProcedureSimpleSyntaxRule rule = ProcedureSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create procedure node
		shared_ptr<ASTNode> expectedASTNode = ProcedureASTNode::createProcedureNode(procedureName);

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstASTNode = StatementListASTNode::createStatementListNode();
		// Create assign node
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();

		// Create variable node
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(variable);

		// Create constant node
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);

		assignASTNode->addChild(variableNode);
		assignASTNode->addChild(constantNode);

		stmtLstASTNode->addChild(assignASTNode);

		expectedASTNode->addChild(stmtLstASTNode);

		test(rule, expectedASTNode);
	}

}