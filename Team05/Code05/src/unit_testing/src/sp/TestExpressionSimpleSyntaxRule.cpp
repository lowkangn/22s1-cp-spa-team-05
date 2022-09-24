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

#include <sp/parser/rules/ExpressionSimpleSyntaxRule.h>


TEST_CASE("ExpressionSimpleSyntaxRule::consumeTokens") {

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

	SECTION("ExpressionSimpleSyntaxRule: Consumes exactly correct tokens #1") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("x"),
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
				Token::createPlusToken(),
				Token::createIntegerToken("2")
		};
		list<Token> expectedTokens = {};
		test(ExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ExpressionSimpleSyntaxRule: Consumes exactly correct tokens #2") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("y"),
				Token::createNameOrKeywordToken("z"), // Illegal operator
				Token::createIntegerToken("2"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createIntegerToken("10")
		};
		list<Token> expectedTokens = {};
		testThrowsException(ExpressionSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("ExpressionSimpleSyntaxRule::generateChildRules") {

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

	SECTION("ExpressionSimpleSyntaxRule: x % 5") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("x"),
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("x") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createModulusToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createIntegerToken("5") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ExpressionSimpleSyntaxRule: x % 5 + 6") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("x"),
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
				Token::createPlusToken(),
				Token::createIntegerToken("6")
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("x") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createModulusToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createIntegerToken("5"), Token::createPlusToken(), Token::createIntegerToken("6") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("ExpressionSimpleSyntaxRule :: constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("Test #1") {
		// x % 5;
		Token xToken = Token::createNameOrKeywordToken("x");
		Token modToken = Token::createModulusToken();
		Token fiveToken = Token::createIntegerToken("5");
		list<Token> tokensToConsume = { xToken, modToken, fiveToken };

		// Create rule
		ExpressionSimpleSyntaxRule rule = ExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(modToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(fiveToken));

		expectedASTNode->addChild(xNode);
		expectedASTNode->addChild(constantNode);

		test(rule, expectedASTNode);
	}

}