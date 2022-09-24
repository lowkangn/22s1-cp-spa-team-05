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

#include <sp/parser/rules/WhileSimpleSyntaxRule.h>


TEST_CASE("WhileSimpleSyntaxRule::consumeTokens") {

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

	SECTION("WhileSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createWhileToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x")
		};
		list<Token> expectedTokens = { Token::createReadToken(),
									   Token::createNameOrKeywordToken("x") };
		test(WhileSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("WhileSimpleSyntaxRule: Consumes exactly correct tokens, with a nested condition") {
		list<Token> tokens = {
				Token::createWhileToken(),
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
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x")
		};
		list<Token> expectedTokens = { Token::createReadToken(),
									   Token::createNameOrKeywordToken("x") };
		test(WhileSimpleSyntaxRule(), tokens, expectedTokens);
	}

}

TEST_CASE("WhileSimpleSyntaxRule::generateChildRules") {

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
	
	SECTION("WhileSimpleSyntaxRule: standard condition") {
		list<Token> tokensToConsume = {
				Token::createWhileToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
		};

		// create conditional rule
		shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInConRule = {
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken() };
		conRule->consumeTokens(tokensInConRule);

		// Create stmtList rule
		shared_ptr<SimpleSyntaxRule> stmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> tokensInStmtLstRule = { Token::createOpenCurlyBracketToken(),
											Token::createReadToken(),
											Token::createNameOrKeywordToken("x"),
											Token::createCloseCurlyBracketToken() };
		stmtLstRule->consumeTokens(tokensInStmtLstRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				conRule,
				stmtLstRule
		};
		test(WhileSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
}

TEST_CASE("WhileSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("WhileSimpleSyntaxRule : constructNode") {
		// while (x > y) then { read x; } else  { read y;}
		Token whileToken = Token::createWhileToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token semiColonToken = Token::createSemicolonToken();

		list<Token> tokensToConsume = { Token::createWhileToken(),
										Token::createOpenBracketToken(),
										variableX,
										greaterThan,
										variableY,
										Token::createCloseBracketToken(),
										Token::createOpenCurlyBracketToken(),
										readToken,
										variableX,
										semiColonToken,
										Token::createCloseCurlyBracketToken(), };

		// Create rule
		WhileSimpleSyntaxRule rule = WhileSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create while node
		shared_ptr<ASTNode> expectedASTNode(new WhileASTNode(whileToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		readNode->addChild(xTwoNode);
		stmtLstNode->addChild(readNode);

		expectedASTNode->addChild(condNode);
		expectedASTNode->addChild(stmtLstNode);

		test(rule, expectedASTNode);
	}

}