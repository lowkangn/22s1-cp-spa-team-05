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

#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>


TEST_CASE("ConstantValueSimpleSyntaxRule :: consumeTokens") {

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

	SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, numeric") {
		list<Token> tokens = { Token::createIntegerToken("1") };
		list<Token> expectedTokens = {};
		test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, very large number") {
		list<Token> tokens = { Token::createIntegerToken("12345435") };
		list<Token> expectedTokens = {};
		test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ConstantValueSimpleSyntaxRule: Multiple tokens, does not throw") {
		list<Token> tokens = { Token::createIntegerToken("12345435"), Token::createIntegerToken("12345435") };
		list<Token> expectedTokens = { Token::createIntegerToken("12345435") };
		test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, not purely numeric, throws") {
		list<Token> tokens = { Token::createSemicolonToken() };
		testThrowsException(ConstantValueSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("ConstantValueSimpleSyntaxRule :: generateChildRules") {

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

	SECTION("ConstValueSimpleSyntaxRule: is terminal, no rules to be generated") {
		list<Token> tokensToConsume = { Token::createIntegerToken("1") };
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};
		test(ConstantValueSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("ConstantValueSimpleSyntaxRule :: constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("ConstantValueSimpleSyntaxRule : constructNode") {
		Token constantValueToken = Token::createIntegerToken("1");
		list<Token> tokensToConsume = { constantValueToken };
		ConstantValueSimpleSyntaxRule rule = ConstantValueSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create constant node
		shared_ptr<ASTNode> expectedASTNode = ConstantValueASTNode::createConstantNode(constantValueToken);

		test(rule, expectedASTNode);
	}

}