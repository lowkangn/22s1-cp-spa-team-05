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

#include <sp/parser/rules/NameSimpleSyntaxRule.h>


TEST_CASE("NameSimpleSyntaxRule::consumeTokens") {

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

	SECTION("NameSimpleSyntaxRule: Exactly one token left, valid") {
		list<Token> tokens = { Token::createNameOrKeywordToken("soomevariable") };
		list<Token> expectedTokens = {};
		test(NameSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("NameSimpleSyntaxRule: Multiple tokens, does not throw") {
		list<Token> tokens = { Token::createNameOrKeywordToken("soomevariable"), Token::createNameOrKeywordToken("soomevariable") };
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("soomevariable") };
		test(NameSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("NameSimpleSyntaxRule: Exactly one token left, not name token, throws") {
		list<Token> tokens = { Token::createSemicolonToken() };
		testThrowsException(NameSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("NameSimpleSyntaxRule::generateChildRules") {

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

	SECTION("NameSimpleSyntaxRule: is terminal, no rules to be generated") {
		list<Token> tokensToConsume = { Token::createNameOrKeywordToken("a") };
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};
		test(NameSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("NameSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("NameSimpleSyntaxRule : constructNode") {
		Token variableValueToken = Token::createNameOrKeywordToken("1");
		list<Token> tokensToConsume = { variableValueToken };
		NameSimpleSyntaxRule rule = NameSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create variable node
		shared_ptr<ASTNode> expectedASTNode(new VariableASTNode(variableValueToken));

		test(rule, expectedASTNode);
	}

}