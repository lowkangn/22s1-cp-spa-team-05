#include <catch.hpp>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/parser/SimpleSyntaxParserManager.cpp>
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>

#include <list>
#include <string>
#include <memory>
using namespace std;

TEST_CASE("ParserManager::parse works correctly") {

	auto test = [](list<Token> tokens, shared_ptr<ASTNode> expected) {

		// ===== given =====
		ParserManager parser = ParserManager(tokens);

		// ===== when =====
		shared_ptr<ASTNode> root = parser.parse();

		// ===== then =====
		REQUIRE(root->equals(*expected));
	};

	SECTION("Parsing a simple program") {
		/*
		 *	  procedure someprocedure {
		 * 1.    x = 1;
		 *    }
		 */
		
		// create token list with variable alias'
		Token procedureKeywordToken = Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token procedureNameToken = Token("someprocedure", TokenType::NAME_OR_KEYWORD);
		Token openCurlyBracketToken = Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER);
		Token xVariableToken = Token("x", TokenType::NAME_OR_KEYWORD);
		Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
		Token integerToken = Token("1", TokenType::INTEGER);
		Token semiColonToken = Token(SEMI_COLON, TokenType::DELIMITER);
		Token closedCurlyBracketToken = Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER);

		list<Token> tokens = {
			procedureKeywordToken,
			procedureNameToken,
			openCurlyBracketToken,
			xVariableToken,
			equalsToken,
			integerToken,
			semiColonToken,
			closedCurlyBracketToken,
		};

		// create the correct ASTNode
		// 1. the root program node
		shared_ptr<ASTNode> expectedProgramNode(new ASTNode(vector<Token>{}));
		expectedProgramNode->setType(ASTNodeType::PROGRAM);

		// 2. program node has a procedure node as sole child 
		shared_ptr<ASTNode> procedureNode(new ASTNode(vector<Token>{procedureKeywordToken}));
		procedureNode->setType(ASTNodeType::PROCEDURE);
		expectedProgramNode->addChild(procedureNode);

		// 3. procedure node has left child as name, right child as statement list node
		shared_ptr<ASTNode> procedureNameNode(new ASTNode(vector<Token>{procedureNameToken}));
		procedureNameNode->setType(ASTNodeType::NAME);
		procedureNode->addChild(procedureNameNode);

		shared_ptr<ASTNode> stmtLstNode(new ASTNode(vector<Token>{}));
		stmtLstNode->setType(ASTNodeType::STMTLIST);
		procedureNode->addChild(stmtLstNode);

		// 4. statement list node has one child, which is an assign node
		shared_ptr<ASTNode> assignNode(new ASTNode(vector<Token>{equalsToken}));
		assignNode->setType(ASTNodeType::ASSIGN);
		assignNode->setLineNumber(1);

		// 5. assign node has left child as x and right child as 1
		shared_ptr<ASTNode> xVariableNode(new ASTNode(vector<Token>{xVariableToken}));
		xVariableNode->setType(ASTNodeType::NAME);
		xVariableNode->setLineNumber(1);
		assignNode->addChild(xVariableNode);

		shared_ptr<ASTNode> integerNode(new ASTNode(vector<Token>{integerToken}));
		integerNode->setType(ASTNodeType::CONSTANT);
		integerNode->setLineNumber(1);
		assignNode->addChild(integerNode);

		// test
		test(tokens, expectedProgramNode);

		
	}
}