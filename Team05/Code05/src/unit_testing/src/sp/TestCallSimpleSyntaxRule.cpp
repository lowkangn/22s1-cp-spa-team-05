#include "catch.hpp"

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>

#include <sp/dataclasses/tokens/Token.h>
#include <list>
#include <memory>

#include <sp/parser/rules/CallSimpleSyntaxRule.h>

TEST_CASE("CallSimpleSyntaxRule::consumeTokens") {

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

	SECTION("CallSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createCallToken(),
				Token::createNameOrKeywordToken("main"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(CallSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("CallSimpleSyntaxRule: Missing call token") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("main"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(CallSimpleSyntaxRule(), tokens);
	}
	SECTION("CallSimpleSyntaxRule: Missing name token") {
		list<Token> tokens = {
				Token::createCallToken(),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(CallSimpleSyntaxRule(), tokens);
	}
	SECTION("CallSimpleSyntaxRule: Missing semicolon token") {
		list<Token> tokens = {
				Token::createCallToken(),
				Token::createNameOrKeywordToken("main"),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(CallSimpleSyntaxRule(), tokens);
	}

}

TEST_CASE("CallSimpleSyntaxRule::generateChildRules") {

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

	SECTION("CallSimpleSyntaxRule: is terminal, no rules to be generated") {
		list<Token> tokensToConsume = { 
				Token::createCallToken(), 
				Token::createNameOrKeywordToken("main"), 
				Token::createSemicolonToken() };

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};

		test(CallSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

}

TEST_CASE("CallSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("CallSimpleSyntaxRule : constructNode") {
		// call someprocedure;
		Token callToken = Token::createCallToken();
		Token procedure = Token::createNameOrKeywordToken("someprocedure");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { callToken,  procedure, semiColon };

		CallSimpleSyntaxRule rule = CallSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create call Node
		shared_ptr<ASTNode> expectedASTNode = CallASTNode::createCallNode();
		// Create procedure Node
		shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(procedure);
		shared_ptr<ASTNode> stmtListNode = StatementListASTNode::createStatementListNode();
		procedureNode->addChild(stmtListNode);

		expectedASTNode->addChild(procedureNode);

		test(rule, expectedASTNode);
	}

}
