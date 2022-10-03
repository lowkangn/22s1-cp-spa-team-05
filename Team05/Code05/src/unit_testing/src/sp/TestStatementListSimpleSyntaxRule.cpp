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

#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>


TEST_CASE("StatementListSimpleSyntaxRule::consumeTokens") {

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

	SECTION("StatementListSimpleSyntaxRule: Consumes exactly correct tokens, using example with just an assign") {
		list<Token> tokens = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		list<Token> expectedTokens = {};
		test(StatementListSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("StatementListSimpleSyntaxRule: Missing {") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(StatementListSimpleSyntaxRule(), tokens);
	}
	SECTION("StatementListSimpleSyntaxRule: Missing }") {
		list<Token> tokens = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};
		testThrowsException(StatementListSimpleSyntaxRule(), tokens);
	}

	SECTION("StatementListSimpleSyntaxRule: Empty Statement list") {
		list<Token> tokens = {
		Token::createOpenCurlyBracketToken(),
		Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(StatementListSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("StatementListSimpleSyntaxRule::generateChildRules") {

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

	SECTION("StatementListSimpleSyntaxRule: nested assignment statement") {
		// tokens, an assign statement in a procedure
		list<Token> tokensToConsume = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// create child rules
		shared_ptr<SimpleSyntaxRule> childRule1 = shared_ptr<SimpleSyntaxRule>(new AssignSimpleSyntaxRule());
		list<Token> tokensInChildRule = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};
		childRule1->consumeTokens(tokensInChildRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				childRule1,
		};
		test(StatementListSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("StatementListSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("StatementListSimpleSyntaxRule : constructNode") {

		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token equalsToken = Token::createEqualsToken();
		Token constantToken = Token::createIntegerToken("1");

		list<Token> tokensToConsume = {
				Token::createOpenCurlyBracketToken(),
				variable,
				equalsToken,
				constantToken,
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// Create rule
		StatementListSimpleSyntaxRule rule = StatementListSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		//Create StmtList node
		shared_ptr<ASTNode> expectedASTNode = StatementListASTNode::createStatementListNode();
		//Create Assign node
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();

		// Create LHS
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(variable);

		// Create RHS
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);

		assignASTNode->addChild(variableNode);
		assignASTNode->addChild(constantNode);

		expectedASTNode->addChild(assignASTNode);

		test(rule, expectedASTNode);
	}

}