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

#include <sp/parser/rules/RelationalExpressionSimpleSyntaxRule.h>


TEST_CASE("RelationalExpressionSimpleSyntaxRule::consumeTokens") {

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

	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
		};
		list<Token> expectedTokens = {};
		test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("y"),
		};
		list<Token> expectedTokens = { };
		test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("y"),
		};
		list<Token> expectedTokens = { Token::createPlusToken(),
									   Token::createNameOrKeywordToken("y") };
		testThrowsException(RelationalExpressionSimpleSyntaxRule(), tokens);
	}

	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualityToken(),
				Token::createIntegerToken("1"),
		};
		list<Token> expectedTokens = {};
		test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

}

TEST_CASE("RelationalExpressionSimpleSyntaxRule::generateChildRules") {

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

TEST_CASE("RelationalExpressionSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("RelationalExpressionSimpleSyntaxRule : constructNode") {
		// x >= y;
		Token xToken = Token::createNameOrKeywordToken("x");
		Token greaterToken = Token::createGreaterThanEqualToken();
		Token yToken = Token::createNameOrKeywordToken("y");
		list<Token> tokensToConsume = { xToken, greaterToken, yToken };

		// Create rule
		RelationalExpressionSimpleSyntaxRule rule = RelationalExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(greaterToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));

		expectedASTNode->addChild(xNode);
		expectedASTNode->addChild(yNode);

		test(rule, expectedASTNode);
	}

}