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

#include <sp/parser/rules/ConditionalExpressionSimpleSyntaxRule.h>


TEST_CASE("ConditionalExpressionSimpleSyntaxRule :: consumeTokens") {

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

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, just a relational operator test") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, just a relational operator test") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createPlaceholderToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, Not test") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createNotToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, two relational expressions") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, doesnt consume extra tokens") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};
		list<Token> expectedTokens = { };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens (!((1==0) && (1==0))), doesnt consume extra tokens") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createNotToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createIntegerToken("1"),
				Token::createEqualityToken(),
				Token::createIntegerToken("0"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createIntegerToken("1"),
				Token::createEqualityToken(),
				Token::createIntegerToken("0"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};
		list<Token> expectedTokens = { };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens (1>=1%((0-1))), doesnt consume extra tokens") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createIntegerToken("1"),
				Token::createGreaterThanEqualToken(),
				Token::createIntegerToken("1"),
				Token::createModulusToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createIntegerToken("0"),
				Token::createMinusToken(),
				Token::createIntegerToken("1"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};
		list<Token> expectedTokens = { };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens (1>=1%((1))), doesnt consume extra tokens") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createIntegerToken("1"),
				Token::createGreaterThanEqualToken(),
				Token::createIntegerToken("1"),
				Token::createModulusToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createIntegerToken("1"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};
		list<Token> expectedTokens = { };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, nested conditional expressions") {
		// ((v >= x) || (q >= x)) && (soomevariable >= x)
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};
		list<Token> expectedTokens = { };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}
}

TEST_CASE("ConditionalExpressionSimpleSyntaxRule :: generateChildRules") {

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

	SECTION("ConditionalExpressionSimpleSyntaxRule: x >= 5") {
		list<Token> tokensToConsume = {
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken()
		};

		// create relational rule
		shared_ptr<SimpleSyntaxRule> relationalRule = shared_ptr<SimpleSyntaxRule>(new RelationalExpressionSimpleSyntaxRule());
		list<Token> tokensRule = { Token::createNameOrKeywordToken("soomevariable"),Token::createGreaterThanEqualToken(),
								   Token::createNameOrKeywordToken("x"), };
		relationalRule->consumeTokens(tokensRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				relationalRule,
		};

		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: !(soomevariable >= x)") {
		list<Token> tokensToConsume = {
				Token::createNotToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
		};

		// create conditional rule
		shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = {
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
		};
		conRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				conRule
		};
		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: (soomevariable >= x) && (soomevariable >= y)") {
		list<Token> tokensToConsume = {
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("y"),
				Token::createCloseBracketToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInLHSRule = {
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
		};
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createAndToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("y"), };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: ((v >= x) || (q >= x)) && ((a >= b) || (c >= d))") {
		list<Token> tokensToConsume = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("a"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("b"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("c"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("d"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("v"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
										Token::createOrToken(),
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("q"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken() };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createAndToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("a"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("b"),
										Token::createCloseBracketToken(),
										Token::createOrToken(),
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("c"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("d"),
										Token::createCloseBracketToken() };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("ConditionalExpressionSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("ConditionalExpressionSimpleSyntaxRule : constructNode") {
		// (x >= y) && (x >= y);
		Token openBracket = Token::createOpenBracketToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token closedBracket = Token::createCloseBracketToken();
		Token andToken = Token::createAndToken();

		list<Token> tokensToConsume = { openBracket, variableX, greaterThan, variableY, closedBracket,
										andToken,
										openBracket, variableX, greaterThan, variableY, closedBracket };

		// Create rule
		ConditionalExpressionSimpleSyntaxRule rule = ConditionalExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(andToken));

		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> greaterThanOneNode(new ExpressionASTNode(greaterThan));

		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yTwoNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> greaterThanTwoNode(new ExpressionASTNode(greaterThan));


		expectedASTNode->addChild(greaterThanOneNode);
		expectedASTNode->addChild(greaterThanTwoNode);

		greaterThanOneNode->addChild(xOneNode);
		greaterThanOneNode->addChild(yOneNode);

		greaterThanTwoNode->addChild(xTwoNode);
		greaterThanTwoNode->addChild(yTwoNode);

		test(rule, expectedASTNode);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule : constructNode") {
		// !((1==2) && (3==4));
		Token openBracket = Token::createOpenBracketToken();
		Token constOneToken = Token::createIntegerToken("1");
		Token constTwoToken = Token::createIntegerToken("2");
		Token constThreeToken = Token::createIntegerToken("3");
		Token constFourToken = Token::createIntegerToken("4");
		Token equalityToken = Token::createEqualityToken();
		Token closedBracket = Token::createCloseBracketToken();
		Token andToken = Token::createAndToken();
		Token notToken = Token::createNotToken();

		list<Token> tokensToConsume = { notToken, openBracket, openBracket, constOneToken, equalityToken, constTwoToken, closedBracket ,
										andToken,
										openBracket, constThreeToken, equalityToken, constFourToken, closedBracket, closedBracket };

		// Create rule
		ConditionalExpressionSimpleSyntaxRule rule = ConditionalExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(andToken));

		shared_ptr<ASTNode> OneNode(new ConstantValueASTNode(constOneToken));
		shared_ptr<ASTNode> TwoNode(new ConstantValueASTNode(constTwoToken));
		shared_ptr<ASTNode> eqaulityNodeOne(new ExpressionASTNode(equalityToken));

		shared_ptr<ASTNode> ThreeNode(new ConstantValueASTNode(constThreeToken));
		shared_ptr<ASTNode> FourNode(new ConstantValueASTNode(constFourToken));
		shared_ptr<ASTNode> eqaulityNodeTwo(new ExpressionASTNode(equalityToken));



		expectedASTNode->addChild(eqaulityNodeOne);
		expectedASTNode->addChild(eqaulityNodeTwo);

		eqaulityNodeOne->addChild(OneNode);
		eqaulityNodeOne->addChild(TwoNode);

		eqaulityNodeTwo->addChild(ThreeNode);
		eqaulityNodeTwo->addChild(FourNode);

		test(rule, expectedASTNode);
	}

}