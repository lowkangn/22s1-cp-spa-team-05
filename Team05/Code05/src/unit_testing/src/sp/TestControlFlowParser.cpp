#include "catch.hpp"

#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/tokens/Token.h>

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>
#include <sp/parser/cfg_parser/ControlFlowGraphparser.h>

#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>


#include <vector>
#include <memory>
#include <istream>
#include <sstream>

using namespace std;

TEST_CASE("addChildToTheEndOfRoot test") {
	auto test = [](shared_ptr<CFGNode> toAdd, shared_ptr<CFGNode> toAddTo, shared_ptr<CFGNode> expected) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		cfgParser.addChildToTheEndOfRoot(toAddTo, toAdd);

		// Then
		REQUIRE(toAddTo->equals(expected));
	};

	SECTION("Simple test") {
		shared_ptr<CFGNode> root = CFGNode::createCFGNode(1);
		shared_ptr<CFGNode> child = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> expected = CFGNode::createCFGNode(1);
		shared_ptr<CFGNode> expectedChild = CFGNode::createCFGNode(2);

		expected->addChild(expectedChild);

		test(child, root, expected);
	}

	SECTION("Traverse till end test") {
		shared_ptr<CFGNode> root = CFGNode::createCFGNode(1);
		shared_ptr<CFGNode> rootChild = CFGNode::createCFGNode(2);
		root->addChild(rootChild);

		shared_ptr<CFGNode> childToAdd = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> expected = CFGNode::createCFGNode(1);
		shared_ptr<CFGNode> expectedRootChild = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> expectedChild = CFGNode::createCFGNode(3);

		expected->addChild(expectedRootChild);
		expectedRootChild->addChild(expectedChild);

		test(childToAdd, root, expected);
	}

	SECTION("Test if") {
		shared_ptr<CFGNode> root = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> thenRoot = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> thenStatement = CFGNode::createCFGNode(3);
		thenRoot->addChild(thenStatement);

		shared_ptr<CFGNode> elseRoot = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> elseStatement = CFGNode::createCFGNode(5);
		elseRoot->addChild(elseStatement);

		shared_ptr<CFGNode> childToAdd = CFGNode::createCFGNode(6);

		root->addChild(thenRoot);
		root->addChild(elseRoot);

		shared_ptr<CFGNode> expected = IfCFGNode::createIfCFGNode(1);
		
		shared_ptr<CFGNode> expectedThenRoot = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> expectedThenStatement = CFGNode::createCFGNode(3);
		expectedThenRoot->addChild(expectedThenStatement);

		shared_ptr<CFGNode> expectedElseRoot = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> expectedElseStatement = CFGNode::createCFGNode(5);
		expectedElseRoot->addChild(expectedElseStatement);

		shared_ptr<CFGNode> expectedChild = CFGNode::createCFGNode(6);

		expectedThenStatement->addChild(expectedChild);
		expectedElseStatement->addChild(expectedChild);

		expected->addChild(expectedThenRoot);
		expected->addChild(expectedElseRoot);

		test(childToAdd, root, expected);
	}

	SECTION("Test while with statements") {
		shared_ptr<CFGNode> root = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> whileStmtOne = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> whileStmtTwo = CFGNode::createCFGNode(3);
		root->addChild(whileStmtOne);
		whileStmtOne->addChild(whileStmtTwo);
		whileStmtTwo->addChild(root);

		shared_ptr<CFGNode> childToAdd = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> expectedRoot = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> expectedWhileStmtOne = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> expectedWhileStmtTwo = CFGNode::createCFGNode(3);
		expectedRoot->addChild(expectedWhileStmtOne);
		expectedWhileStmtOne->addChild(expectedWhileStmtTwo);
		expectedWhileStmtTwo->addChild(expectedRoot);

		shared_ptr<CFGNode> expectedChild = CFGNode::createCFGNode(4);

		expectedRoot->addChild(expectedChild);

		test(childToAdd, root, expectedRoot);
	}
}

TEST_CASE("handleStatementList test") {
	auto test = [](shared_ptr<ASTNode> statementListASTNode, shared_ptr<CFGNode> expectedCFGNode) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleStatementList(statementListASTNode);

		// Then
		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	SECTION("Statement only test") {
		/*
			{
				x = 5;
				read x;
				print y;
			}
		*/

		shared_ptr<ASTNode> stmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// assign
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(Token::createEqualsToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> constant(new ConstantValueASTNode(Token::createIntegerToken("5")));
		assignASTNode->addChild(xNode);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		// read
		shared_ptr<ASTNode> readASTNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xNode2(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		readASTNode->addChild(xNode2);
		readASTNode->setLineNumber(2);

		// print
		shared_ptr<ASTNode> printASTNode(new PrintASTNode(Token::createPrintToken()));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		printASTNode->addChild(yNode);
		printASTNode->setLineNumber(3);


		stmtListASTNode->addChild(assignASTNode);
		stmtListASTNode->addChild(readASTNode);
		stmtListASTNode->addChild(printASTNode);

		shared_ptr<CFGNode> expected = CFGNode::createCFGNode(1);
		shared_ptr<CFGNode> expectedRead = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> expectedPrint = CFGNode::createCFGNode(3);

		expected->addChild(expectedRead);
		expectedRead->addChild(expectedPrint);

		test(stmtListASTNode, expected);
	}
	
	SECTION("Statement with if test") {
		/*
			{
				1. x = 5;
				2. if (x >= y) then {
				3.	print x;
				}  else {
				4.	print y;
				}
				5. read x;
			}
		*/

		shared_ptr<ASTNode> stmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// assign
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(Token::createEqualsToken()));
		shared_ptr<ASTNode> xNodeAssign(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> constant(new ConstantValueASTNode(Token::createIntegerToken("5")));
		assignASTNode->addChild(xNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		//If
		shared_ptr<ASTNode> ifNode(new IfASTNode(Token::createIfToken()));

		shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		ifNode->setLineNumber(2);
		conditionNode->setLineNumber(2);
		xNode->setLineNumber(2);
		yNode-> setLineNumber(2);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> thenStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printX(new PrintASTNode(Token::createPrintToken()));
		thenStmtListASTNode->addChild(printX);
		printX->setLineNumber(3);

		shared_ptr<ASTNode> elseStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		elseStmtListASTNode->addChild(printY);
		printY->setLineNumber(4);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		shared_ptr<ASTNode> readX(new ReadASTNode(Token::createReadToken()));
		readX->setLineNumber(5);

		stmtListASTNode->addChild(assignASTNode);
		stmtListASTNode->addChild(ifNode);
		stmtListASTNode->addChild(readX);

		// CFG Node
		shared_ptr<CFGNode> expected = CFGNode::createCFGNode(1);

		shared_ptr<CFGNode> ifCFG = IfCFGNode::createIfCFGNode(2);

		shared_ptr<CFGNode> expectedPrintX = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> expectedPrintY = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> expectedRead = CFGNode::createCFGNode(5);

		expected->addChild(ifCFG);

		ifCFG->addChild(expectedPrintX);
		ifCFG->addChild(expectedPrintY);

		expectedPrintX->addChild(expectedRead);
		expectedPrintY->addChild(expectedRead);

		test(stmtListASTNode, expected);
	}

	SECTION("Statement with while test") {
		/*
			{
				1. x = 5;
				2. while (x >= y) {
				3.	print x;
				} 
				4. read x;
			}
		*/
		shared_ptr<ASTNode> stmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// assign
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(Token::createEqualsToken()));
		shared_ptr<ASTNode> xNodeAssign(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> constant(new ConstantValueASTNode(Token::createIntegerToken("5")));
		assignASTNode->addChild(xNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		//While
		shared_ptr<ASTNode> whileNode(new WhileASTNode(Token::createWhileToken()));

		shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		whileNode->setLineNumber(2);
		conditionNode->setLineNumber(2);
		xNode->setLineNumber(2);
		yNode->setLineNumber(2);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> whileStatementList(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		whileStatementList->addChild(printY);
		printY->setLineNumber(3);

		whileNode->addChild(conditionNode);
		whileNode->addChild(whileStatementList);

		shared_ptr<ASTNode> readX(new ReadASTNode(Token::createReadToken()));
		readX->setLineNumber(4);

		stmtListASTNode->addChild(assignASTNode);
		stmtListASTNode->addChild(whileNode);
		stmtListASTNode->addChild(readX);

		// CFG Node
		shared_ptr<CFGNode> expected = CFGNode::createCFGNode(1);

		shared_ptr<CFGNode> whileCFG = WhileCFGNode::createWhileCFGNode(2);

		shared_ptr<CFGNode> expectedPrintY = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> expectedRead = CFGNode::createCFGNode(4);

		expected->addChild(whileCFG);

		whileCFG->addChild(expectedPrintY);
		expectedPrintY->addChild(whileCFG);

		whileCFG->addChild(expectedRead);

		test(stmtListASTNode, expected);
	}

	// Then and else must point to start of while
	SECTION("Nested if inside while test") {
		// If inside while 

		/*
			{
			1.	while (x >= y) {
			2.		if (z == 2) then {
			3.			print x;
					} else {
			4.			read x;
					}
				}
			5.	x = 5;
			}
		*/
		shared_ptr<ASTNode> stmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// While
		shared_ptr<ASTNode> whileNode(new WhileASTNode(Token::createWhileToken()));

		shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		whileNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> whileStatementList(new StatementListASTNode(Token::createPlaceholderToken()));

		//If
		shared_ptr<ASTNode> ifNode(new IfASTNode(Token::createIfToken()));

		shared_ptr<ASTNode> ifConditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> zNode(new VariableASTNode(Token::createNameOrKeywordToken("z")));
		shared_ptr<ASTNode> twoNode(new VariableASTNode(Token::createIntegerToken("2")));
		ifNode->setLineNumber(2);
		ifConditionNode->setLineNumber(2);
		zNode->setLineNumber(2);
		twoNode->setLineNumber(2);

		ifConditionNode->addChild(zNode);
		ifConditionNode->addChild(twoNode);

		shared_ptr<ASTNode> thenStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printX(new PrintASTNode(Token::createPrintToken()));
		thenStmtListASTNode->addChild(printX);
		printX->setLineNumber(3);

		shared_ptr<ASTNode> elseStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> readX(new ReadASTNode(Token::createReadToken()));
		elseStmtListASTNode->addChild(readX);
		readX->setLineNumber(4);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		whileStatementList->addChild(ifNode);

		whileNode->addChild(conditionNode);
		whileNode->addChild(whileStatementList);

		// assign
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(Token::createEqualsToken()));
		shared_ptr<ASTNode> xNodeAssign(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> constant(new ConstantValueASTNode(Token::createIntegerToken("5")));
		assignASTNode->addChild(xNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(5);

		stmtListASTNode->addChild(whileNode);
		stmtListASTNode->addChild(assignASTNode);

		// CFG
		shared_ptr<CFGNode> expected = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> ifCFG = IfCFGNode::createIfCFGNode(2);

		shared_ptr<CFGNode> printCFG = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> readCFG = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> assignCFG = CFGNode::createCFGNode(5);

		expected->addChild(ifCFG);

		ifCFG->addChild(printCFG);
		ifCFG->addChild(readCFG);

		printCFG->addChild(expected);
		readCFG->addChild(expected);

		expected->addChild(assignCFG);

		test(stmtListASTNode, expected);
	}
}

TEST_CASE("handleIf test") {
	auto test = [](shared_ptr<ASTNode> ifASTNode, shared_ptr<CFGNode> expectedCFGNode) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleIf(ifASTNode);

		// Then
		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	SECTION("If with one statement in then and else") {
		//If
		shared_ptr<ASTNode> ifNode(new IfASTNode(Token::createIfToken()));

		shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		ifNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> thenStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printX(new PrintASTNode(Token::createPrintToken()));
		thenStmtListASTNode->addChild(printX);
		printX->setLineNumber(2);

		shared_ptr<ASTNode> elseStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		elseStmtListASTNode->addChild(printY);
		printY->setLineNumber(3);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		// CFGNode
		shared_ptr<CFGNode> ifCFG = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> expectedPrintX = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> expectedPrintY = CFGNode::createCFGNode(3);

		ifCFG->addChild(expectedPrintX);
		ifCFG->addChild(expectedPrintY);

		test(ifNode, ifCFG);
	}

	SECTION("Nested if") {
		/*
			1. if (x >= y) then {
			2.   if (q >= z) then {
			3.		print x;
				 } else {
			4.		print y;
				 }
			   } else {
			5.	read x;
			}
		*/
		//If
		shared_ptr<ASTNode> ifNode(new IfASTNode(Token::createIfToken()));

		shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		ifNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> thenStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> ifTwoNode(new IfASTNode(Token::createIfToken()));
		shared_ptr<ASTNode> conditionNodeTwo(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> qNode(new VariableASTNode(Token::createNameOrKeywordToken("q")));
		shared_ptr<ASTNode> zNode(new VariableASTNode(Token::createNameOrKeywordToken("z")));
		ifTwoNode->setLineNumber(2);
		conditionNodeTwo->setLineNumber(2);
		qNode->setLineNumber(2);
		zNode->setLineNumber(2);

		conditionNodeTwo->addChild(qNode);
		conditionNodeTwo->addChild(zNode);

		shared_ptr<ASTNode> thenStmtListASTNodeTwo(new StatementListASTNode(Token::createPlaceholderToken()));
		
		shared_ptr<ASTNode> printX(new PrintASTNode(Token::createPrintToken()));
		thenStmtListASTNodeTwo->addChild(printX);
		printX->setLineNumber(3);

		shared_ptr<ASTNode> elseStmtListASTNodeTwo(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		elseStmtListASTNodeTwo->addChild(printY);
		printY->setLineNumber(4);

		ifTwoNode->addChild(conditionNodeTwo);
		ifTwoNode->addChild(thenStmtListASTNodeTwo);
		ifTwoNode->addChild(elseStmtListASTNodeTwo);

		thenStmtListASTNode->addChild(ifTwoNode);

		shared_ptr<ASTNode> elseStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> readX(new ReadASTNode(Token::createReadToken()));
		readX->setLineNumber(5);

		elseStmtListASTNode->addChild(readX);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		// CFGNode
		shared_ptr<CFGNode> ifCFG = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> ifCFGTwo = IfCFGNode::createIfCFGNode(2);

		shared_ptr<CFGNode> expectedPrintX = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> expectedPrintY = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> expectedReadX = CFGNode::createCFGNode(5);

		ifCFGTwo->addChild(expectedPrintX);
		ifCFGTwo->addChild(expectedPrintY);

		ifCFG->addChild(ifCFGTwo);
		ifCFG->addChild(expectedReadX);

		test(ifNode, ifCFG);
	} 
}

TEST_CASE("handleWhile test") {
	auto test = [](shared_ptr<ASTNode> whileASTNode, shared_ptr<CFGNode> expectedCFGNode) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleWhile(whileASTNode);

		// Then
		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	// Test recursion
	SECTION("While with one statement in loop") {
		/*
			while (x >= y) {
				print y;
			}
		*/
		// While
		shared_ptr<ASTNode> whileNode(new WhileASTNode(Token::createWhileToken()));

		shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		whileNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> whileStatementList(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		whileStatementList->addChild(printY);
		printY->setLineNumber(2);

		whileNode->addChild(conditionNode);
		whileNode->addChild(whileStatementList);

		shared_ptr<CFGNode> whileCFG = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> expectedPrintY = CFGNode::createCFGNode(2);

		whileCFG->addChild(expectedPrintY);

		test(whileNode, whileCFG);
	}

	SECTION("Nested while") {
		/*
			1. while (x >= 5) {
			2. 	while (y >= 5) {
			3. 		print y;
				}
			}
		*/

		// While
		shared_ptr<ASTNode> whileNode(new WhileASTNode(Token::createWhileToken()));

		shared_ptr<ASTNode> conditionOneNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> fiveNode(new ConstantValueASTNode(Token::createIntegerToken("5")));
		whileNode->setLineNumber(1);
		conditionOneNode->setLineNumber(1);
		xNode->setLineNumber(1);
		fiveNode->setLineNumber(1);

		conditionOneNode->addChild(xNode);
		conditionOneNode->addChild(fiveNode);

		shared_ptr<ASTNode> whileStatementList(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> whileTwoNode(new WhileASTNode(Token::createWhileToken()));
		shared_ptr<ASTNode> conditionTwoNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		shared_ptr<ASTNode> fiveTwoNode(new ConstantValueASTNode(Token::createIntegerToken("5")));
		whileTwoNode->setLineNumber(2);
		conditionTwoNode->setLineNumber(2);
		yNode->setLineNumber(2);
		fiveTwoNode->setLineNumber(2);

		conditionTwoNode->addChild(yNode);
		conditionTwoNode->addChild(fiveTwoNode);

		whileTwoNode->addChild(conditionTwoNode);

		shared_ptr<ASTNode> whileTwoStatementList(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		whileTwoStatementList->addChild(printY);
		printY->setLineNumber(3);

		whileTwoNode->addChild(whileTwoStatementList);

		whileStatementList->addChild(whileTwoNode);

		whileNode->addChild(conditionOneNode);
		whileNode->addChild(whileStatementList);

		// CFGNode
		shared_ptr<CFGNode> expected = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> whileCFG = WhileCFGNode::createWhileCFGNode(2);

		shared_ptr<CFGNode> expectedPrintY = CFGNode::createCFGNode(3);

		expected->addChild(whileCFG);
		whileCFG->addChild(expectedPrintY);
		expectedPrintY->addChild(whileCFG);
		whileCFG->addChild(expected);

		test(whileNode, expected);
	}
}

TEST_CASE("handleProcedure test") {
	auto test = [](shared_ptr<ASTNode> procNode, shared_ptr<CFGNode> expectedCFGNode) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleProcedure(procNode);

		// Then
		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	SECTION("Single procedure") {
		/*
			procedure main {
			1.	while (x >= 5) {
			2.		read x;
				}
			3.	if (y == 2) then {
			4.		print y;
				} else {
			5.		read z;
				}
			6.	q = 5;
			}
		*/
		shared_ptr<ASTNode> procNode(new ProcedureASTNode(Token::createNameOrKeywordToken("main")));


		shared_ptr<ASTNode> procStatementList(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> whileNode(new WhileASTNode(Token::createWhileToken()));
		shared_ptr<ASTNode> conditionOneNode(new ExpressionASTNode(Token::createGreaterThanEqualToken()));
		shared_ptr<ASTNode> xNode(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> fiveNode(new ConstantValueASTNode(Token::createIntegerToken("5")));
		whileNode->setLineNumber(1);
		conditionOneNode->setLineNumber(1);
		xNode->setLineNumber(1);
		fiveNode->setLineNumber(1);

		conditionOneNode->addChild(xNode);
		conditionOneNode->addChild(fiveNode);

		shared_ptr<ASTNode> whileStatementList(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> whileRead(new ReadASTNode(Token::createReadToken()));
		whileRead->setLineNumber(2);
		whileStatementList->addChild(whileRead);

		whileNode->addChild(conditionOneNode);
		whileNode->addChild(whileStatementList);

		shared_ptr<ASTNode> ifNode(new IfASTNode(Token::createWhileToken()));
		shared_ptr<ASTNode> conditionNodeTwo(new ExpressionASTNode(Token::createEqualityToken()));
		shared_ptr<ASTNode> yNode(new VariableASTNode(Token::createNameOrKeywordToken("y")));
		shared_ptr<ASTNode> twoNode(new ConstantValueASTNode(Token::createIntegerToken("2")));
		ifNode->setLineNumber(3);
		conditionNodeTwo->setLineNumber(3);
		yNode->setLineNumber(3);
		twoNode->setLineNumber(3);

		conditionNodeTwo->addChild(yNode);
		conditionNodeTwo->addChild(twoNode);

		shared_ptr<ASTNode> thenStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> printY(new PrintASTNode(Token::createPrintToken()));
		thenStmtListASTNode->addChild(printY);
		printY->setLineNumber(4);

		shared_ptr<ASTNode> elseStmtListASTNode(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> readZ(new ReadASTNode(Token::createReadToken()));
		elseStmtListASTNode->addChild(readZ);
		readZ->setLineNumber(5);

		ifNode->addChild(conditionNodeTwo);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		// assign
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(Token::createEqualsToken()));
		shared_ptr<ASTNode> qNodeAssign(new VariableASTNode(Token::createNameOrKeywordToken("q")));
		shared_ptr<ASTNode> constant(new ConstantValueASTNode(Token::createIntegerToken("5")));
		assignASTNode->addChild(qNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		procStatementList->addChild(whileNode);
		procStatementList->addChild(ifNode);
		procStatementList->addChild(assignASTNode);

		procNode->addChild(procStatementList);

		// CFGs
		shared_ptr<CFGNode> expected = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> readX = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> ifCFG = IfCFGNode::createIfCFGNode(3);

		shared_ptr<CFGNode> printYCFG = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> readZCFG = CFGNode::createCFGNode(5);

		shared_ptr<CFGNode> assignCFG = CFGNode::createCFGNode(6);

		expected->addChild(readX);
		readX->addChild(expected);
		expected->addChild(ifCFG);

		ifCFG->addChild(printYCFG);
		ifCFG->addChild(readZCFG);

		printYCFG->addChild(assignCFG);
		readZCFG->addChild(assignCFG);

		test(procNode, expected);
	}
}

TEST_CASE("parse") {
	auto test = [](shared_ptr<ASTNode> programNode, vector<shared_ptr<CFGNode>> expectedCFGNodes) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		vector<shared_ptr<CFGNode>> generatedCFGNodes = cfgParser.parse(programNode);

		// Then
		REQUIRE(generatedCFGNodes.size() == expectedCFGNodes.size());

		for (int i = 0; i < generatedCFGNodes.size(); i++) {
			REQUIRE(generatedCFGNodes[i]->equals(expectedCFGNodes[i]));
		}
	};
	
	SECTION("Multiple procedures") {
		string program = "procedure main {\n\twhile (x >= 5) {\n\t\tx = 10;\n\t}\n\tread x;\n}\n\nprocedure pain {\n\tif (x != 10) then {\n\t\tprint y;\n\t} else {\n\t\tx = 10 + y;\n\t}\n\tread x;\n}";

		stringstream ss(program);
		istream& stream = ss;

		Lexer lexer = Lexer();
		list<Token> tokens = lexer.tokenize(stream);

		ParserManager parser = ParserManager(tokens);
		shared_ptr<ASTNode> programTree = parser.parse();

		// First CFG
		shared_ptr<CFGNode> firstCFG = WhileCFGNode::createWhileCFGNode(1);
		shared_ptr<CFGNode> firstAssign = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> firstRead = CFGNode::createCFGNode(3);

		firstCFG->addChild(firstAssign);
		firstAssign->addChild(firstCFG);
		firstCFG->addChild(firstRead);

		// Second CFG
		shared_ptr<CFGNode> secondCFG = IfCFGNode::createIfCFGNode(4);
		shared_ptr<CFGNode> secondPrint = CFGNode::createCFGNode(5);
		shared_ptr<CFGNode> secondAssign = CFGNode::createCFGNode(6);
		shared_ptr<CFGNode> secondRead = CFGNode::createCFGNode(7);

		secondCFG->addChild(secondPrint);
		secondCFG->addChild(secondAssign);

		secondPrint->addChild(secondRead);
		secondAssign->addChild(secondRead);

		// Expected CFGs
		vector<shared_ptr<CFGNode>> expected {firstCFG, secondCFG};

		test(programTree, expected);
	}
}
