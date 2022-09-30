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

#include <sp/parser/rules/IfSimpleSyntaxRule.h>


TEST_CASE("IfSimpleSyntaxRule::consumeTokens") {

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

	SECTION("IfSimpleSyntaxRule: no else") {
		list<Token> tokens = {
			Token::createIfToken(),
			Token::createOpenBracketToken(),
			Token::createNameOrKeywordToken("x"),
			Token::createOperatorToken(">"),
			Token::createNameOrKeywordToken("y"),
			Token::createCloseBracketToken(),
			Token::createThenToken(),
			Token::createOpenCurlyBracketToken(),
			Token::createPrintToken(),
			Token::createNameOrKeywordToken("x"),
			Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(IfSimpleSyntaxRule(), tokens);
	}

	SECTION("IfSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createIfToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createThenToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createElseToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("x"),
		};
		list<Token> expectedTokens = { Token::createPrintToken(),
									   Token::createNameOrKeywordToken("x"), };
		test(IfSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("IfSimpleSyntaxRule: Consumes exactly correct tokens, with a nested condition") {
		list<Token> tokens = {
				Token::createIfToken(),
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
				Token::createThenToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createElseToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("x"),
		};
		list<Token> expectedTokens = { Token::createPrintToken(),
									   Token::createNameOrKeywordToken("x"), };
		test(IfSimpleSyntaxRule(), tokens, expectedTokens);
	}
}

TEST_CASE("IfSimpleSyntaxRule::generateChildRules") {

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


	SECTION("IfSimpleSyntaxRule: standard condition") {
		list<Token> tokensToConsume = {
				Token::createIfToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createThenToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createElseToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
		};

		// create conditional rule
		shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInConRule = { Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken() };
		conRule->consumeTokens(tokensInConRule);

		// Create stmtList rule
		shared_ptr<SimpleSyntaxRule> thenStmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> tokensInThenStmtLstRule = { Token::createOpenCurlyBracketToken(),
												Token::createReadToken(),
												Token::createNameOrKeywordToken("x"),
												Token::createCloseCurlyBracketToken() };
		thenStmtLstRule->consumeTokens(tokensInThenStmtLstRule);

		shared_ptr<SimpleSyntaxRule> elseStmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> tokensInElseStmtLstRule = { Token::createOpenCurlyBracketToken(),
												Token::createReadToken(),
												Token::createNameOrKeywordToken("x"),
												Token::createCloseCurlyBracketToken() };
		elseStmtLstRule->consumeTokens(tokensInElseStmtLstRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				conRule,
				thenStmtLstRule,
				elseStmtLstRule
		};
		test(IfSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
	
}

TEST_CASE("IfSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	SECTION("IfSimpleSyntaxRule : constructNode") {
		// if (x > y) then { read x; } else  { read y;}
		Token ifToken = Token::createIfToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token semiColonToken = Token::createSemicolonToken();

		list<Token> tokensToConsume = { ifToken,
										Token::createOpenBracketToken(),
										variableX,
										greaterThan,
										variableY,
										Token::createCloseBracketToken(),
										Token::createThenToken(),
										Token::createOpenCurlyBracketToken(),
										readToken,
										variableX,
										semiColonToken,
										Token::createCloseCurlyBracketToken(),
										Token::createElseToken(),
										Token::createOpenCurlyBracketToken(),
										readToken,
										variableY,
										semiColonToken,
										Token::createCloseCurlyBracketToken(), };

		// Create rule
		IfSimpleSyntaxRule rule = IfSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create while node
		shared_ptr<ASTNode> expectedASTNode = IfASTNode::createIfNode();

		// Create condition node
		shared_ptr<ASTNode> xOneNode = VariableASTNode::createVariableNode(variableX);
		shared_ptr<ASTNode> yOneNode = VariableASTNode::createVariableNode(variableY);
		shared_ptr<ASTNode> condNode = ExpressionASTNode::createExpressionNode(greaterThan);

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create then stmtlst node
		shared_ptr<ASTNode> stmtLstOneNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readOneNode = ReadASTNode::createReadNode();		
		shared_ptr<ASTNode> xTwoNode = VariableASTNode::createVariableNode(variableX);

		// Create else stmtlst node
		shared_ptr<ASTNode> stmtLstTwoNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readTwoNode = ReadASTNode::createReadNode();		
		shared_ptr<ASTNode> yTwoNode = VariableASTNode::createVariableNode(variableY);

		readOneNode->addChild(xTwoNode);
		stmtLstOneNode->addChild(readOneNode);

		readTwoNode->addChild(yTwoNode);
		stmtLstTwoNode->addChild(readTwoNode);

		expectedASTNode->addChild(condNode);
		expectedASTNode->addChild(stmtLstOneNode);
		expectedASTNode->addChild(stmtLstTwoNode);

		test(rule, expectedASTNode);
	}

}