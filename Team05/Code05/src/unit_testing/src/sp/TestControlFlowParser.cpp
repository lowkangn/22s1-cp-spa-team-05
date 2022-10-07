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
#include <sp/parser/cfg_parser/ControlFlowGraphParser.h>

#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>


#include <vector>
#include <unordered_map>
#include <memory>
#include <istream>
#include <sstream>

using namespace std;

TEST_CASE("addChildToEndOfNode test") {
	auto test = [](shared_ptr<CFGNode> toAdd, shared_ptr<CFGNode> toAddTo, shared_ptr<CFGNode> expected) {
		// Given
		ControlFlowParser cfgParser = ControlFlowParser();

		// When
		cfgParser.addChildToEndOfNode(toAddTo, toAdd);

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

		shared_ptr<ASTNode> stmtListASTNode = StatementListASTNode::createStatementListNode();

		// assign
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();		
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> constant = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		assignASTNode->addChild(xNode);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		// read
		shared_ptr<ASTNode> readASTNode = ReadASTNode::createReadNode();		
		shared_ptr<ASTNode> xNode2 = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		readASTNode->addChild(xNode2);
		readASTNode->setLineNumber(2);

		// print
		shared_ptr<ASTNode> printASTNode = PrintASTNode::createPrintNode();		
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		printASTNode->addChild(yNode);
		printASTNode->setLineNumber(3);


		stmtListASTNode->addChild(assignASTNode);
		stmtListASTNode->addChild(readASTNode);
		stmtListASTNode->addChild(printASTNode);



		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1 ,CFGNode::createCFGNode(1)},
			{2, CFGNode::createCFGNode(2)},
			{3, CFGNode::createCFGNode(3)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3}}
		};

		shared_ptr<CFGNode> expected = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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

		shared_ptr<ASTNode> stmtListASTNode = StatementListASTNode::createStatementListNode();

		// assign
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();		
		shared_ptr<ASTNode> xNodeAssign = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> constant = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		assignASTNode->addChild(xNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		//If
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();

		shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		ifNode->setLineNumber(2);
		conditionNode->setLineNumber(2);
		xNode->setLineNumber(2);
		yNode-> setLineNumber(2);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> thenStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printX = PrintASTNode::createPrintNode();		
		thenStmtListASTNode->addChild(printX);
		printX->setLineNumber(3);

		shared_ptr<ASTNode> elseStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();		
		elseStmtListASTNode->addChild(printY);
		printY->setLineNumber(4);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		shared_ptr<ASTNode> readX = ReadASTNode::createReadNode();		
		readX->setLineNumber(5);

		stmtListASTNode->addChild(assignASTNode);
		stmtListASTNode->addChild(ifNode);
		stmtListASTNode->addChild(readX);

		// CFG Node
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, CFGNode::createCFGNode(1)},
			{2, IfCFGNode::createIfCFGNode(2)},
			// Print x
			{3, CFGNode::createCFGNode(3)},
			// Print y
			{4, CFGNode::createCFGNode(4)},
			// read
			{5, CFGNode::createCFGNode(5)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3, 4}},
			{3, {5}},
			{4, {5}},
		};

		shared_ptr<CFGNode> expected = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> stmtListASTNode = StatementListASTNode::createStatementListNode();

		// assign
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();		shared_ptr<ASTNode> xNodeAssign = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> constant = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		assignASTNode->addChild(xNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(1);

		//While
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();

		shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		whileNode->setLineNumber(2);
		conditionNode->setLineNumber(2);
		xNode->setLineNumber(2);
		yNode->setLineNumber(2);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> whileStatementList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();
		whileStatementList->addChild(printY);
		printY->setLineNumber(3);

		whileNode->addChild(conditionNode);
		whileNode->addChild(whileStatementList);

		shared_ptr<ASTNode> readX = ReadASTNode::createReadNode();
		readX->setLineNumber(4);

		stmtListASTNode->addChild(assignASTNode);
		stmtListASTNode->addChild(whileNode);
		stmtListASTNode->addChild(readX);

		// CFG Node
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, CFGNode::createCFGNode(1)},
			{2, WhileCFGNode::createWhileCFGNode(2)},
			// Print y
			{3, CFGNode::createCFGNode(3)},
			// Read
			{4, CFGNode::createCFGNode(4)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3,4}},
			{3, {2}}
		};

		shared_ptr<CFGNode> expected = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> stmtListASTNode = StatementListASTNode::createStatementListNode();

		// While
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();

		shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		whileNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> whileStatementList = StatementListASTNode::createStatementListNode();

		//If
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();

		shared_ptr<ASTNode> ifConditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> zNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("z"));
		shared_ptr<ASTNode> twoNode = VariableASTNode::createVariableNode(Token::createIntegerToken("2"));
		ifNode->setLineNumber(2);
		ifConditionNode->setLineNumber(2);
		zNode->setLineNumber(2);
		twoNode->setLineNumber(2);

		ifConditionNode->addChild(zNode);
		ifConditionNode->addChild(twoNode);

		shared_ptr<ASTNode> thenStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printX = PrintASTNode::createPrintNode();		
		thenStmtListASTNode->addChild(printX);
		printX->setLineNumber(3);

		shared_ptr<ASTNode> elseStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readX = ReadASTNode::createReadNode();		
		elseStmtListASTNode->addChild(readX);
		readX->setLineNumber(4);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		whileStatementList->addChild(ifNode);

		whileNode->addChild(conditionNode);
		whileNode->addChild(whileStatementList);

		// assign
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();		shared_ptr<ASTNode> xNodeAssign = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> constant = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		assignASTNode->addChild(xNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(5);

		stmtListASTNode->addChild(whileNode);
		stmtListASTNode->addChild(assignASTNode);

		// CFG
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			{2, IfCFGNode::createIfCFGNode(2)},
			// Print 
			{3, CFGNode::createCFGNode(3)},
			// Read
			{4, CFGNode::createCFGNode(4)},
			// Assign
			{5, CFGNode::createCFGNode(5)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2,5}},
			{2, {3,4}},
			{3, {1}},
			{4, {1}},
		};

		shared_ptr<CFGNode> expected = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();

		shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		ifNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> thenStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printX = PrintASTNode::createPrintNode();		
		thenStmtListASTNode->addChild(printX);
		printX->setLineNumber(2);

		shared_ptr<ASTNode> elseStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();		
		elseStmtListASTNode->addChild(printY);
		printY->setLineNumber(3);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		// CFGNode
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, IfCFGNode::createIfCFGNode(1)},
			// Print x
			{2, CFGNode::createCFGNode(2)},
			// Print y
			{3, CFGNode::createCFGNode(3)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2,3}},
		};

		shared_ptr<CFGNode> ifCFG = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();

		shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		ifNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> thenStmtListASTNode = StatementListASTNode::createStatementListNode();

		shared_ptr<ASTNode> ifTwoNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> conditionNodeTwo = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> qNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("q"));
		shared_ptr<ASTNode> zNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("z"));
		ifTwoNode->setLineNumber(2);
		conditionNodeTwo->setLineNumber(2);
		qNode->setLineNumber(2);
		zNode->setLineNumber(2);

		conditionNodeTwo->addChild(qNode);
		conditionNodeTwo->addChild(zNode);

		shared_ptr<ASTNode> thenStmtListASTNodeTwo = StatementListASTNode::createStatementListNode();
		
		shared_ptr<ASTNode> printX = PrintASTNode::createPrintNode();		
		thenStmtListASTNodeTwo->addChild(printX);
		printX->setLineNumber(3);

		shared_ptr<ASTNode> elseStmtListASTNodeTwo = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();		
		elseStmtListASTNodeTwo->addChild(printY);
		printY->setLineNumber(4);

		ifTwoNode->addChild(conditionNodeTwo);
		ifTwoNode->addChild(thenStmtListASTNodeTwo);
		ifTwoNode->addChild(elseStmtListASTNodeTwo);

		thenStmtListASTNode->addChild(ifTwoNode);

		shared_ptr<ASTNode> elseStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readX = ReadASTNode::createReadNode();		readX->setLineNumber(5);

		elseStmtListASTNode->addChild(readX);

		ifNode->addChild(conditionNode);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		// CFGNode
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, IfCFGNode::createIfCFGNode(1)},
			{2, IfCFGNode::createIfCFGNode(2)},
			// Print x
			{3, CFGNode::createCFGNode(3)},
			// Print y
			{4, CFGNode::createCFGNode(4)},
			// Read x
			{5, CFGNode::createCFGNode(5)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2,5}},
			{2, {3,4}},
		};

		shared_ptr<CFGNode> ifCFG = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();

		shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		whileNode->setLineNumber(1);
		conditionNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);

		conditionNode->addChild(xNode);
		conditionNode->addChild(yNode);

		shared_ptr<ASTNode> whileStatementList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();		
		whileStatementList->addChild(printY);
		printY->setLineNumber(2);

		whileNode->addChild(conditionNode);
		whileNode->addChild(whileStatementList);

		// CFG
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			{2, CFGNode::createCFGNode(2)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {1}},
		};

		shared_ptr<CFGNode> whileCFG = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();

		shared_ptr<ASTNode> conditionOneNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> fiveNode = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		whileNode->setLineNumber(1);
		conditionOneNode->setLineNumber(1);
		xNode->setLineNumber(1);
		fiveNode->setLineNumber(1);

		conditionOneNode->addChild(xNode);
		conditionOneNode->addChild(fiveNode);

		shared_ptr<ASTNode> whileStatementList = StatementListASTNode::createStatementListNode();

		shared_ptr<ASTNode> whileTwoNode = WhileASTNode::createWhileNode();
		shared_ptr<ASTNode> conditionTwoNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		shared_ptr<ASTNode> fiveTwoNode = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		whileTwoNode->setLineNumber(2);
		conditionTwoNode->setLineNumber(2);
		yNode->setLineNumber(2);
		fiveTwoNode->setLineNumber(2);

		conditionTwoNode->addChild(yNode);
		conditionTwoNode->addChild(fiveTwoNode);

		whileTwoNode->addChild(conditionTwoNode);

		shared_ptr<ASTNode> whileTwoStatementList = StatementListASTNode::createStatementListNode();

		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();		
		whileTwoStatementList->addChild(printY);
		printY->setLineNumber(3);

		whileTwoNode->addChild(whileTwoStatementList);

		whileStatementList->addChild(whileTwoNode);

		whileNode->addChild(conditionOneNode);
		whileNode->addChild(whileStatementList);

		// CFGNode
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			{2, WhileCFGNode::createWhileCFGNode(2)},
			// PrintY
			{3, WhileCFGNode::createWhileCFGNode(3)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3, 1}},
			{3, {2}},
		};

		shared_ptr<CFGNode> expected = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		shared_ptr<ASTNode> procNode = ProcedureASTNode::createProcedureNode(Token::createNameOrKeywordToken("main"));


		shared_ptr<ASTNode> procStatementList = StatementListASTNode::createStatementListNode();

		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();
		shared_ptr<ASTNode> conditionOneNode = ExpressionASTNode::createExpressionNode(Token::createGreaterThanEqualToken());
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> fiveNode = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		whileNode->setLineNumber(1);
		conditionOneNode->setLineNumber(1);
		xNode->setLineNumber(1);
		fiveNode->setLineNumber(1);

		conditionOneNode->addChild(xNode);
		conditionOneNode->addChild(fiveNode);

		shared_ptr<ASTNode> whileStatementList = StatementListASTNode::createStatementListNode();

		shared_ptr<ASTNode> whileRead = ReadASTNode::createReadNode();		
		whileRead->setLineNumber(2);
		whileStatementList->addChild(whileRead);

		whileNode->addChild(conditionOneNode);
		whileNode->addChild(whileStatementList);

		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> conditionNodeTwo = ExpressionASTNode::createExpressionNode(Token::createEqualityToken());
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("y"));
		shared_ptr<ASTNode> twoNode = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("2"));
		ifNode->setLineNumber(3);
		conditionNodeTwo->setLineNumber(3);
		yNode->setLineNumber(3);
		twoNode->setLineNumber(3);

		conditionNodeTwo->addChild(yNode);
		conditionNodeTwo->addChild(twoNode);

		shared_ptr<ASTNode> thenStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printY = PrintASTNode::createPrintNode();		
		thenStmtListASTNode->addChild(printY);
		printY->setLineNumber(4);

		shared_ptr<ASTNode> elseStmtListASTNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readZ = ReadASTNode::createReadNode();		
		elseStmtListASTNode->addChild(readZ);
		readZ->setLineNumber(5);

		ifNode->addChild(conditionNodeTwo);
		ifNode->addChild(thenStmtListASTNode);
		ifNode->addChild(elseStmtListASTNode);

		// assign
		shared_ptr<ASTNode> assignASTNode = AssignASTNode::createAssignNode();		
		shared_ptr<ASTNode> qNodeAssign = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("q"));
		shared_ptr<ASTNode> constant = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("5"));
		assignASTNode->addChild(qNodeAssign);
		assignASTNode->addChild(constant);
		assignASTNode->setLineNumber(6);

		procStatementList->addChild(whileNode);
		procStatementList->addChild(ifNode);
		procStatementList->addChild(assignASTNode);

		procNode->addChild(procStatementList);

		// CFGs
		unordered_map<int, shared_ptr<CFGNode>> CFGNodes = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// Read X
			{2, CFGNode::createCFGNode(2)},
			// If
			{3, IfCFGNode::createIfCFGNode(3)},
			// Print y
			{4, CFGNode::createCFGNode(4)},
			// read z
			{5, CFGNode::createCFGNode(5)},
			// assign
			{6, CFGNode::createCFGNode(6)},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 3}},
			{2, {1}},
			{3, {4, 5}},
			{4, {6}},
			{5, {6}},
		};

		shared_ptr<CFGNode> expected = CFGNode::createCFGFromAdjacencyList(CFGNodes, adjList, 1);

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
		unordered_map<int, shared_ptr<CFGNode>> firstCFGNodes = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// assign
			{2, CFGNode::createCFGNode(2)},
			// read
			{3, CFGNode::createCFGNode(3)},
		};

		unordered_map<int, vector<int>> firstAdjList = {
			{1, {2, 3}},
			{2, {1}},
		};

		shared_ptr<CFGNode> firstCFG = CFGNode::createCFGFromAdjacencyList(firstCFGNodes, firstAdjList, 1);

		// Second CFG
		unordered_map<int, shared_ptr<CFGNode>> secondCFGNodes = {
			{4, IfCFGNode::createIfCFGNode(4)},
			// print
			{5, CFGNode::createCFGNode(5)},
			// assign
			{6, CFGNode::createCFGNode(6)},
			// Read
			{7, CFGNode::createCFGNode(7)},
		};

		unordered_map<int, vector<int>> secondAdjList = {
			{4, {5, 6}},
			{5, {7}},
			{6, {7}},
		};

		shared_ptr<CFGNode> secondCFG = CFGNode::createCFGFromAdjacencyList(secondCFGNodes, secondAdjList, 4);

		// Expected CFGs
		vector<shared_ptr<CFGNode>> expected {firstCFG, secondCFG};

		test(programTree, expected);
	}
}
