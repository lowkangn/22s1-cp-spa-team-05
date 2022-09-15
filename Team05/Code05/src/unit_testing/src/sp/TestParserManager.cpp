#include <catch.hpp>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
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
		REQUIRE(root->equals(expected));
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
		shared_ptr<ASTNode> expectedProgramNode(new ProgramASTNode(Token{ PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD }));

		// 2. program node has a procedure node as sole child 
		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(procedureNameToken));
		expectedProgramNode->addChild(procedureNode);

		shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));
		procedureNode->addChild(stmtLstNode);

		// 4. statement list node has one child, which is an assign node
		shared_ptr<ASTNode> assignNode(new AssignASTNode(equalsToken));
		assignNode->setLineNumber(1);
		stmtLstNode->addChild(assignNode);

		// 5. assign node has left child as x and right child as 1
		shared_ptr<ASTNode> xVariableNode(new VariableASTNode(xVariableToken));
		xVariableNode->setLineNumber(1);
		assignNode->addChild(xVariableNode);

		shared_ptr<ASTNode> integerNode(new ConstantValueASTNode(integerToken));
		integerNode->setLineNumber(1);
		assignNode->addChild(integerNode);

		// test
		test(tokens, expectedProgramNode);


	}
}