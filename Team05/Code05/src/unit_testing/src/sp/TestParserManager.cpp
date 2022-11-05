#include <catch.hpp>
#include <sp/lexer/Lexer.h>
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
#include <istream>
#include <sstream>
using namespace std;

TEST_CASE("ParserManager::parse works correctly") {

	auto test = [](list<Token> tokens, shared_ptr<ASTNode> expected) {

		// ===== given =====
		ParserManager parser = ParserManager();

		// ===== when =====
		shared_ptr<ASTNode> root = parser.parse(tokens);

		// ===== then =====
		REQUIRE(root->equals(expected));
	};

	auto testThrows = [](list<Token> tokens, shared_ptr<ASTNode> expected) {

		// ===== given =====
		ParserManager parser = ParserManager();

		// ===== when && then =====
		REQUIRE_THROWS(parser.parse(tokens));
	};

	SECTION("Parsing a simple program") {
		/*
		 *	  procedure someprocedure {
		 * 1.    x = 1;
		 *    }
		 */

		 // create token list with variable alias'
		Token procedureKeywordToken = Token::createProcedureToken();
		Token procedureNameToken = Token::createNameOrKeywordToken("someprocedure");
		Token openCurlyBracketToken = Token::createOpenCurlyBracketToken();
		Token xVariableToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token integerToken = Token::createIntegerToken("1");
		Token semiColonToken = Token::createSemicolonToken();
		Token closedCurlyBracketToken = Token::createCloseCurlyBracketToken();

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
		shared_ptr<ASTNode> expectedProgramNode = ProgramASTNode::createProgramNode();
		// 2. program node has a procedure node as sole child 
		shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(procedureNameToken);
		expectedProgramNode->addChild(procedureNode);

		shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();		
		procedureNode->addChild(stmtLstNode);

		// 4. statement list node has one child, which is an assign node
		shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();
		assignNode->setLineNumber(1);
		stmtLstNode->addChild(assignNode);

		// 5. assign node has left child as x and right child as 1
		shared_ptr<ASTNode> xVariableNode = VariableASTNode::createVariableNode(xVariableToken);
		xVariableNode->setLineNumber(1);
		assignNode->addChild(xVariableNode);

		shared_ptr<ASTNode> integerNode = ConstantValueASTNode::createConstantNode(integerToken);
		integerNode->setLineNumber(1);
		assignNode->addChild(integerNode);


		// test
		test(tokens, expectedProgramNode);
	}

	SECTION("Parsing an invalid program with while containing a semi-colon at the end") {
		Lexer lexer = Lexer();
		string program = "procedure main {\n\twhile (x > 1) {\n\t\tx = 1;\t\n\t};\n}";

		stringstream ss(program);
		istream& stream = ss;

		list<Token> tokens = lexer.tokenize(ss);

		shared_ptr<ASTNode> expectedProgramNode = ProgramASTNode::createProgramNode();		
		testThrows(tokens, expectedProgramNode);
	}

	SECTION("Parsing program with brackets inside expressions") {
		Lexer lexer = Lexer();
		string program = "procedure z { \
							while ((k) == 10) {\
								read x;\
							}\
						}";

		stringstream ss(program);
		istream& stream = ss;

		list<Token> tokens = lexer.tokenize(ss);

		shared_ptr<ASTNode> expectedProgramNode = ProgramASTNode::createProgramNode();

		// 2. program node has a procedure node as sole child 
		shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(Token::createNameOrKeywordToken("z"));
		expectedProgramNode->addChild(procedureNode);

		shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
		procedureNode->addChild(stmtLstNode);

		// While node
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();
		whileNode->setLineNumber(1);

		stmtLstNode->addChild(whileNode);

		// Condition
		shared_ptr<ASTNode> operatorNode = ExpressionASTNode::createExpressionNode(Token::createEqualityToken());
		shared_ptr<ASTNode> bracketsNode = BracketsASTNode::createBracketsNode();
		shared_ptr<ASTNode> kNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("k"));
		shared_ptr<ASTNode> constNode = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("10"));

		operatorNode->setLineNumber(1);
		bracketsNode->setLineNumber(1);
		kNode->setLineNumber(1);
		constNode->setLineNumber(1);
		
		operatorNode->addChild(bracketsNode);
		operatorNode->addChild(constNode);

		bracketsNode->addChild(kNode);
		whileNode->addChild(operatorNode);

		// Statement list
		shared_ptr<ASTNode> whileStmtLstNode = StatementListASTNode::createStatementListNode();
		whileNode->addChild(whileStmtLstNode);

		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();
		shared_ptr<ASTNode> variableXNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		
		readNode->setLineNumber(2);
		variableXNode->setLineNumber(2);

		readNode->addChild(variableXNode);
		whileStmtLstNode->addChild(readNode);

		test(tokens, expectedProgramNode);
	}
}