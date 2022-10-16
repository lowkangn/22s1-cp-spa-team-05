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


TEST_CASE("AssignSimpleSyntaxRule :: consumeTokens") {

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

	SECTION("AssignSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(AssignSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("AssignSimpleSyntaxRule: Consumes exactly correct tokens, expression with extra brackets") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createOpenBracketToken(),
				Token::createIntegerToken("1"),
				Token::createCloseBracketToken(),
				Token::createPlusToken(),
				Token::createIntegerToken("5"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(AssignSimpleSyntaxRule(), tokens, expectedTokens);
	}


	SECTION("AssignSimpleSyntaxRule: Missing = token") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(AssignSimpleSyntaxRule(), tokens);
	}

	SECTION("AssignSimpleSyntaxRule: Missing token after equal") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(AssignSimpleSyntaxRule(), tokens);
	}
	SECTION("AssignSimpleSyntaxRule: Missing semicolon token") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(AssignSimpleSyntaxRule(), tokens);
	}
}

TEST_CASE("AssignSimpleSyntaxRule :: generateChildRules") {

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

	SECTION("AssignSimpleSyntaxRule: nested is the lhs and rhs, should have name lhs and const rhs generated") {
		// tokens, an assign statement
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("soomevariable") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createIntegerToken("1"), };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("AssignSimpleSyntaxRule: expression x = (x) + 1") {
		list<Token> tokensToConsume = {
			Token::createNameOrKeywordToken("x"),
			Token::createEqualsToken(),
			Token::createOpenBracketToken(),
			Token::createNameOrKeywordToken("x"),
			Token::createCloseBracketToken(),
			Token::createPlusToken(),
			Token::createIntegerToken("1"),
			Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("x") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
										Token::createPlusToken(),
										Token::createIntegerToken("1") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("AssignSimpleSyntaxRule: expression x = x + 1") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("soomevariable") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createNameOrKeywordToken("x"), Token::createPlusToken(), Token::createIntegerToken("1") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("AssignSimpleSyntaxRule: expression x = x + 1 - 3") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createIntegerToken("1"),
				Token::createMinusToken(),
				Token::createIntegerToken("3"),
				Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("soomevariable") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createNameOrKeywordToken("x"), Token::createPlusToken(), Token::createIntegerToken("1"), Token::createMinusToken(), Token::createIntegerToken("3"), };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("AssignSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("AssignSimpleSyntaxRule : constructNode") {
		// x = 5;
		Token leftHandSideToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token rightHandSideToken = Token::createIntegerToken("5");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { leftHandSideToken,  equalsToken, rightHandSideToken, semiColon };

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode = AssignASTNode::createAssignNode();

		// Create LHS
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(leftHandSideToken);

		// Create RHS
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(rightHandSideToken);

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(constantNode);

		test(rule, expectedASTNode);
	}

	SECTION("AssignSimpleSyntaxRule : constructNode -> x = (x) + 1") {
		Token xToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token oneToken = Token::createIntegerToken("1");
		Token semiColon = Token::createSemicolonToken();
		Token plusToken = Token::createPlusToken();

		list<Token> tokensToConsume = {
			xToken,
			equalsToken,
			Token::createOpenBracketToken(),
			xToken,
			Token::createCloseBracketToken(),
			plusToken,
			oneToken,
			semiColon,
		};

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode = AssignASTNode::createAssignNode();

		// Create LHS
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(xToken);

		// Create RHS
		shared_ptr<ASTNode> expressionNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> bracketsNode = BracketsASTNode::createBracketsNode();
		shared_ptr<ASTNode> expressionXNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(oneToken);

		expressionNode->addChild(bracketsNode);
		expressionNode->addChild(constantNode);

		bracketsNode->addChild(expressionXNode);

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(expressionNode);

		test(rule, expectedASTNode);
	}

	SECTION("AssignSimpleSyntaxRule : constructNode -> x = x + 1") {
		// x = x + 1;
		Token leftHandSideToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token rightXToken = Token::createNameOrKeywordToken("x");
		Token plusToken = Token::createPlusToken();
		Token rightOneToken = Token::createIntegerToken("1");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { leftHandSideToken,  equalsToken, rightXToken, plusToken, rightOneToken, semiColon };

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode = AssignASTNode::createAssignNode();

		// Create LHS
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(leftHandSideToken);
		shared_ptr<ASTNode> rhsXNode = VariableASTNode::createVariableNode(rightXToken);

		// Create expression
		shared_ptr<ASTNode> expressionNode = ExpressionASTNode::createExpressionNode(plusToken);

		// Create RHS
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(rightOneToken);

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(expressionNode);

		expressionNode->addChild(rhsXNode);
		expressionNode->addChild(constantNode);

		test(rule, expectedASTNode);
	}

	SECTION("AssignSimpleSyntaxRule : constructNode -> x = x + 1 + (1 + x)") {
		// x = x + 1;
		Token leftHandSideToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token rightXToken = Token::createNameOrKeywordToken("x");
		Token plusToken = Token::createPlusToken();
		Token rightOneToken = Token::createIntegerToken("1");
		Token semiColon = Token::createSemicolonToken();
		Token openBracket = Token::createOpenBracketToken();
		Token closedBracket = Token::createCloseBracketToken();
		list<Token> tokensToConsume = { leftHandSideToken,  equalsToken,
										rightXToken, plusToken, rightOneToken, plusToken,
										openBracket, rightOneToken, plusToken, rightXToken, closedBracket, semiColon };

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode = AssignASTNode::createAssignNode();

		// Create LHS
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(leftHandSideToken);
		shared_ptr<ASTNode> rhsXOutNode = VariableASTNode::createVariableNode(rightXToken);
		shared_ptr<ASTNode> rhsXInNode = VariableASTNode::createVariableNode(rightXToken);

		// Create expressions
		shared_ptr<ASTNode> expressionOneNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> expressionTwoNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> expressionBracketNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> bracketASTNode= BracketsASTNode::createBracketsNode();

		// Create RHS
		shared_ptr<ASTNode> constantOutNode = ConstantValueASTNode::createConstantNode(rightOneToken);
		shared_ptr<ASTNode> constantInNode = ConstantValueASTNode::createConstantNode(rightOneToken);

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(expressionOneNode);

		expressionOneNode->addChild(rhsXOutNode);
		expressionOneNode->addChild(expressionTwoNode);

		expressionTwoNode->addChild(constantOutNode);
		expressionTwoNode->addChild(bracketASTNode);

		bracketASTNode->addChild(expressionBracketNode);

		expressionBracketNode->addChild(constantInNode);
		expressionBracketNode->addChild(rhsXInNode);

		test(rule, expectedASTNode);
	}

}