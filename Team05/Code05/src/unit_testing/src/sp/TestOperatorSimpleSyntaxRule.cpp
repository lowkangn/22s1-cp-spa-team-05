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

#include <sp/parser/rules/AssignSimpleSyntaxRule.h>


TEST_CASE("OperatorSimpleSyntaxRule::consumeTokens") {

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

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createPlusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createMinusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createMultiplyToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createDivideToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createModulusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createModulusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
				Token::createPlusToken(),
				Token::createIntegerToken("2")
		};
		list<Token> expectedTokens = { Token::createIntegerToken("5"),
									   Token::createPlusToken(),
									   Token::createIntegerToken("2") };
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

}

TEST_CASE("OperatorSimpleSyntaxRule::generateChildRules") {

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
	
}

TEST_CASE("OperatorSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("OperatorSimpleSyntaxRule : constructNode") {
		// x = 5;
		Token addToken = Token::createPlusToken();
		list<Token> tokensToConsume = { addToken };

		// Create rule
		OperatorSimpleSyntaxRule rule = OperatorSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode = ExpressionASTNode::createExpressionNode(addToken);

		test(rule, expectedASTNode);
	}

}