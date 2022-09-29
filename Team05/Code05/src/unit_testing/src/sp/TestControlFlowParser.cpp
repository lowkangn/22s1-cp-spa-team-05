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

#include <vector>
#include <memory>

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
		shared_ptr<CFGNode> root(new CFGNode(1));
		shared_ptr<CFGNode> child(new CFGNode(2));

		shared_ptr<CFGNode> expected(new CFGNode(1));
		shared_ptr<CFGNode> expectedChild(new CFGNode(2));

		expected->addNext(expectedChild);

		test(child, root, expected);
	}

	SECTION("Traverse till end test") {
		shared_ptr<CFGNode> root(new CFGNode(1));
		shared_ptr<CFGNode> rootChild(new CFGNode(2));
		root->addNext(rootChild);

		shared_ptr<CFGNode> childToAdd(new CFGNode(3));

		shared_ptr<CFGNode> expected(new CFGNode(1));
		shared_ptr<CFGNode> expectedRootChild(new CFGNode(2));
		shared_ptr<CFGNode> expectedChild(new CFGNode(3));

		expected->addNext(expectedRootChild);
		expectedRootChild->addNext(expectedChild);

		test(childToAdd, root, expected);
	}

	SECTION("Test if") {
		shared_ptr<CFGNode> root(new IfCFGNode(1));

		shared_ptr<CFGNode> thenRoot(new CFGNode(2));
		shared_ptr<CFGNode> thenStatement(new CFGNode(3));
		thenRoot->addNext(thenStatement);

		shared_ptr<CFGNode> elseRoot(new CFGNode(4));
		shared_ptr<CFGNode> elseStatement(new CFGNode(5));
		elseRoot->addNext(elseStatement);

		shared_ptr<CFGNode> childToAdd(new CFGNode(6));

		root->addNext(thenRoot);
		root->addNext(elseRoot);

		shared_ptr<CFGNode> expected(new IfCFGNode(1));
		
		shared_ptr<CFGNode> expectedThenRoot(new CFGNode(2));
		shared_ptr<CFGNode> expectedThenStatement(new CFGNode(3));
		expectedThenRoot->addNext(expectedThenStatement);

		shared_ptr<CFGNode> expectedElseRoot(new CFGNode(4));
		shared_ptr<CFGNode> expectedElseStatement(new CFGNode(5));
		expectedElseRoot->addNext(expectedElseStatement);

		shared_ptr<CFGNode> expectedChild(new CFGNode(6));

		expectedThenStatement->addNext(expectedChild);
		expectedElseStatement->addNext(expectedChild);

		expected->addNext(expectedThenRoot);
		expected->addNext(expectedElseRoot);

		test(childToAdd, root, expected);
	}

	SECTION("Test while with statements") {
		shared_ptr<CFGNode> root(new WhileCFGNode(1));

		shared_ptr<CFGNode> whileStmtOne(new CFGNode(2));
		shared_ptr<CFGNode> whileStmtTwo(new CFGNode(3));
		root->addNext(whileStmtOne);
		whileStmtOne->addNext(whileStmtTwo);
		whileStmtTwo->addNext(root);

		shared_ptr<CFGNode> childToAdd(new CFGNode(4));

		shared_ptr<CFGNode> expectedRoot(new WhileCFGNode(1));

		shared_ptr<CFGNode> expectedWhileStmtOne(new CFGNode(2));
		shared_ptr<CFGNode> expectedWhileStmtTwo(new CFGNode(3));
		expectedRoot->addNext(expectedWhileStmtOne);
		expectedWhileStmtOne->addNext(expectedWhileStmtTwo);
		expectedWhileStmtTwo->addNext(expectedRoot);

		shared_ptr<CFGNode> expectedChild(new CFGNode(4));

		expectedRoot->addNext(expectedChild);

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

		shared_ptr<CFGNode> expected(new CFGNode(1));
		shared_ptr<CFGNode> expectedRead(new CFGNode(2));
		shared_ptr<CFGNode> expectedPrint(new CFGNode(3));

		expected->addNext(expectedRead);
		expectedRead->addNext(expectedPrint);

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
		shared_ptr<CFGNode> expected(new CFGNode(1));

		shared_ptr<CFGNode> ifCFG(new IfCFGNode(2));

		shared_ptr<CFGNode> expectedPrintX(new CFGNode(3));
		shared_ptr<CFGNode> expectedPrintY(new CFGNode(4));

		shared_ptr<CFGNode> expectedRead(new CFGNode(5));

		expected->addNext(ifCFG);

		ifCFG->addNext(expectedPrintX);
		ifCFG->addNext(expectedPrintY);

		expectedPrintX->addNext(expectedRead);
		expectedPrintY->addNext(expectedRead);

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
		shared_ptr<CFGNode> expected(new CFGNode(1));

		shared_ptr<CFGNode> whileCFG(new WhileCFGNode(2));

		shared_ptr<CFGNode> expectedPrintY(new CFGNode(3));

		shared_ptr<CFGNode> expectedRead(new CFGNode(4));

		expected->addNext(whileCFG);

		whileCFG->addNext(expectedPrintY);
		expectedPrintY->addNext(whileCFG);

		whileCFG->addNext(expectedRead);

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
		shared_ptr<CFGNode> expected(new WhileCFGNode(1));

		shared_ptr<CFGNode> ifCFG(new IfCFGNode(2));

		shared_ptr<CFGNode> printCFG(new CFGNode(3));
		shared_ptr<CFGNode> readCFG(new CFGNode(4));
		shared_ptr<CFGNode> assignCFG(new CFGNode(5));

		expected->addNext(ifCFG);

		ifCFG->addNext(printCFG);
		ifCFG->addNext(readCFG);

		printCFG->addNext(expected);
		readCFG->addNext(expected);

		expected->addNext(assignCFG);

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
		shared_ptr<CFGNode> ifCFG(new IfCFGNode(1));

		shared_ptr<CFGNode> expectedPrintX(new CFGNode(2));
		shared_ptr<CFGNode> expectedPrintY(new CFGNode(3));

		ifCFG->addNext(expectedPrintX);
		ifCFG->addNext(expectedPrintY);

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
		shared_ptr<CFGNode> ifCFG(new IfCFGNode(1));

		shared_ptr<CFGNode> ifCFGTwo(new IfCFGNode(2));

		shared_ptr<CFGNode> expectedPrintX(new CFGNode(3));
		shared_ptr<CFGNode> expectedPrintY(new CFGNode(4));
		shared_ptr<CFGNode> expectedReadX(new CFGNode(5));

		ifCFGTwo->addNext(expectedPrintX);
		ifCFGTwo->addNext(expectedPrintY);

		ifCFG->addNext(ifCFGTwo);
		ifCFG->addNext(expectedReadX);

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

		shared_ptr<CFGNode> whileCFG(new WhileCFGNode(1));

		shared_ptr<CFGNode> expectedPrintY(new CFGNode(2));

		whileCFG->addNext(expectedPrintY);

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
		shared_ptr<CFGNode> expected(new WhileCFGNode(1));

		shared_ptr<CFGNode> whileCFG(new WhileCFGNode(2));

		shared_ptr<CFGNode> expectedPrintY(new CFGNode(3));

		expected->addNext(whileCFG);
		whileCFG->addNext(expectedPrintY);
		expectedPrintY->addNext(whileCFG);
		whileCFG->addNext(expected);

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

	}
}
