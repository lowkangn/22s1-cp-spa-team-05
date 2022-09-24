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

#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>


TEST_CASE("ReadSimpleSyntaxRule::consumeTokens") {

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

	SECTION("ReadSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(ReadSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ReadSimpleSyntaxRule: Missing read token") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(ReadSimpleSyntaxRule(), tokens);
	}
	SECTION("ReadSimpleSyntaxRule: Missing name token") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(ReadSimpleSyntaxRule(), tokens);
	}
	SECTION("ReadSimpleSyntaxRule: Missing semicolon token") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(ReadSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("ReadSimpleSyntaxRule::generateChildRules") {

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

	SECTION("ReadSimpleSyntaxRule: nested is the name keyword, should have name rule generated") {
		list<Token> tokensToConsume = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
		};
		shared_ptr<SimpleSyntaxRule> nameRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInNameRule = { Token::createNameOrKeywordToken("soomevariable") };
		nameRule->consumeTokens(tokensInNameRule);
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				nameRule
		};
		test(ReadSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("ReadSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("ReadSimpleSyntaxRule : constructNode") {
		// read soomevariable;
		Token readToken = Token::createReadToken();
		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { readToken,  variable, semiColon };

		ReadSimpleSyntaxRule rule = ReadSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create Read Node
		shared_ptr<ASTNode> expectedASTNode(new ReadASTNode(readToken));

		// Create variableNode
		shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));

		expectedASTNode->addChild(variableNode);


		test(rule, expectedASTNode);
	}

}