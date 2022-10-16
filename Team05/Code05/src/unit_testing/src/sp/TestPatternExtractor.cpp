#include "catch.hpp"
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>

#include <vector>
#include <memory>
#include <iostream>

TEST_CASE("PatternExtractor: test handleAssign") {
	auto test = [](shared_ptr<ASTNode> ast, vector<Pattern> expectedResult) {
		// Given
		PatternExtractor extractor = PatternExtractor();

		// When
		vector<Pattern> extractedPattern = extractor.handleAssign(ast);

		// Then
		for (Pattern e : extractedPattern) {
			bool check = find(expectedResult.begin(), expectedResult.end(), e) != expectedResult.end();
			REQUIRE(check);
		}
	};

	SECTION("Test Assign") {
		vector<Pattern> expectedPattern{ Pattern::createAssignPattern(1, " x ", " x 1 + ") };

		Token assignToken = Token::createEqualsToken();
		Token xToken = Token::createNameOrKeywordToken("x");
		Token constantToken = Token::createIntegerToken("1");
		Token plusToken = Token::createPlusToken();

		shared_ptr<ASTNode> assignNode= AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> x1Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> x2Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);
		shared_ptr<ASTNode> expressionNode = ExpressionASTNode::createExpressionNode(plusToken);

		assignNode->addChild(x1Node);
		assignNode->setLineNumber(1);
		assignNode->addChild(expressionNode);

		expressionNode->addChild(x2Node);
		expressionNode->addChild(constantNode);

		test(assignNode, expectedPattern);
	}

	SECTION("Test Assign with brackets") {
		vector<Pattern> expectedPattern{ Pattern::createAssignPattern(1, " x ", " x 1 x + + ") };

		Token assignToken = Token::createEqualsToken();
		Token xToken = Token::createNameOrKeywordToken("x");
		Token constantToken = Token::createIntegerToken("1");
		Token plusToken = Token::createPlusToken();

		shared_ptr<ASTNode> assignNode= AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> x1Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> x2Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);
		shared_ptr<ASTNode> expressionNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> expressionTwoNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> bracketNode= BracketsASTNode::createBracketsNode();

		assignNode->addChild(x1Node);
		assignNode->setLineNumber(1);
		assignNode->addChild(expressionNode);

		bracketNode->addChild(expressionTwoNode);

		expressionTwoNode->addChild(constantNode);
		expressionTwoNode->addChild(x2Node);

		expressionNode->addChild(x2Node);
		expressionNode->addChild(bracketNode);

		test(assignNode, expectedPattern);
	}
}

TEST_CASE("PatternExtractor: test handleIf") {
	auto test = [](shared_ptr<ASTNode> ast, vector<Pattern> expectedResult) {
		// Given
		PatternExtractor extractor = PatternExtractor();

		// When
		vector<Pattern> extractedPattern = extractor.handleIf(ast);

		// Then
		for (int i = 0; i < extractedPattern.size(); i++) {
			REQUIRE(extractedPattern[i].equals(expectedResult[i]));
		}
	};

	SECTION("Test If") {
		// if (x > y) then { read x; } else  { read y;}
		vector<Pattern> expectedPattern{ Pattern::createIfPattern(1, "x y") };

		Token ifToken = Token::createIfToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token semiColonToken = Token::createSemicolonToken();

		// Create if node
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
		expectedASTNode->setLineNumber(1);

		test(expectedASTNode, expectedPattern);
	}
}

TEST_CASE("PatternExtractor: test handleWhile") {
	auto test = [](shared_ptr<ASTNode> ast, vector<Pattern> expectedResult) {
		// Given
		PatternExtractor extractor = PatternExtractor();

		// When
		vector<Pattern> extractedPattern = extractor.handleWhile(ast);

		// Then
		for (int i = 0; i < extractedPattern.size(); i++) {
			REQUIRE(extractedPattern[i].equals(expectedResult[i]));
		}
	};

	SECTION("Test while") {
		vector<Pattern> expectedPattern{ Pattern::createWhilePattern(1, "x y") };

		// while (x > y) then { read x; } else  { read y;}
		Token whileToken = Token::createWhileToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token semiColonToken = Token::createSemicolonToken();

		// Create while node
		shared_ptr<ASTNode> expectedASTNode = WhileASTNode::createWhileNode();
		// Create condition node
		shared_ptr<ASTNode> xOneNode = VariableASTNode::createVariableNode(variableX);
		shared_ptr<ASTNode> yOneNode = VariableASTNode::createVariableNode(variableY);
		shared_ptr<ASTNode> condNode = ExpressionASTNode::createExpressionNode(greaterThan);

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();		shared_ptr<ASTNode> xTwoNode = VariableASTNode::createVariableNode(variableX);

		readNode->addChild(xTwoNode);
		stmtLstNode->addChild(readNode);

		expectedASTNode->addChild(condNode);
		expectedASTNode->addChild(stmtLstNode);
		expectedASTNode->setLineNumber(1);

		test(expectedASTNode, expectedPattern);
	}
}

TEST_CASE("PatternExtractor: test extract") {
	auto test = [](shared_ptr<ASTNode> ast, vector<Pattern> expectedResult) {
		// Given
		PatternExtractor extractor = PatternExtractor();

		// When
		vector<Pattern> extractedPattern = extractor.extract(ast);

		REQUIRE(expectedResult.size() == extractedPattern.size());
		// Then
		for (int i = 0; i < extractedPattern.size(); i++) {
			bool isInExpectedPatterns = false;
			for (int j = 0; j < expectedResult.size(); j++) {
				if (extractedPattern[i].equals(expectedResult[j])) {
					isInExpectedPatterns = true;
					break;
				}
			}
			REQUIRE(isInExpectedPatterns);
		}
	};

	SECTION("Test extract") {
		// if (x > y) then { while (y == 5) { read x } } else  { x = x + (5 + x) ;}
		Token whileToken = Token::createWhileToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token equalityToken = Token::createEqualityToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token programToken = Token::createProgramToken();
		Token ifToken = Token::createIfToken();
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token constantToken = Token::createIntegerToken("5");

		// Create program
		shared_ptr<ASTNode> programASTNode = ProgramASTNode::createProgramNode();
		// Create procedure
		shared_ptr<ASTNode> mainASTNode = ProgramASTNode::createProgramNode();
		programASTNode->addChild(mainASTNode);

		// Procedure stmtlst
		shared_ptr<ASTNode> mainStmtLstNode = StatementListASTNode::createStatementListNode();
		mainASTNode->addChild(mainStmtLstNode);


		// Create while node
		shared_ptr<ASTNode> ifASTNode = IfASTNode::createIfNode();

		// Create condition node
		shared_ptr<ASTNode> xOneNode = VariableASTNode::createVariableNode(variableX);
		shared_ptr<ASTNode> yOneNode = VariableASTNode::createVariableNode(variableY);
		shared_ptr<ASTNode> condNode = ExpressionASTNode::createExpressionNode(greaterThan);

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create then stmtlst node
		shared_ptr<ASTNode> thenStmtLstNode = StatementListASTNode::createStatementListNode();
		// create while node
		shared_ptr<ASTNode> whileASTNode = WhileASTNode::createWhileNode();
		thenStmtLstNode->addChild(whileASTNode);

		// create while condition
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);
		shared_ptr<ASTNode> whileYOneNode = VariableASTNode::createVariableNode(variableY);
		shared_ptr<ASTNode> whileCondNode = ExpressionASTNode::createExpressionNode(equalityToken);

		whileCondNode->addChild(whileYOneNode);
		whileCondNode->addChild(constantNode);

		whileASTNode->addChild(whileCondNode);

		// create while stmtlst node
		shared_ptr<ASTNode> whileStmtLstNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();		
		shared_ptr<ASTNode> xTwoNode = VariableASTNode::createVariableNode(variableX);

		whileStmtLstNode->addChild(readNode);
		whileStmtLstNode->addChild(xTwoNode);

		whileASTNode->addChild(whileStmtLstNode);

		// create else stmtlst node
		shared_ptr<ASTNode> elseStmtLstNode = StatementListASTNode::createStatementListNode();
		// create assign node
		Token assignToken = Token::createEqualsToken();
		Token plusToken = Token::createPlusToken();

		shared_ptr<ASTNode> assignNode= AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> x1Node = VariableASTNode::createVariableNode(variableX);
		shared_ptr<ASTNode> x2Node = VariableASTNode::createVariableNode(variableX);
		shared_ptr<ASTNode> assignConstantNode = ConstantValueASTNode::createConstantNode(constantToken);
		shared_ptr<ASTNode> expressionNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> expressionTwoNode = ExpressionASTNode::createExpressionNode(plusToken);
		shared_ptr<ASTNode> bracketNode= BracketsASTNode::createBracketsNode();

		assignNode->addChild(x1Node);
		assignNode->addChild(expressionNode);

		bracketNode->addChild(expressionTwoNode);

		expressionTwoNode->addChild(assignConstantNode);
		expressionTwoNode->addChild(x2Node);

		expressionNode->addChild(x2Node);
		expressionNode->addChild(bracketNode);

		elseStmtLstNode->addChild(assignNode);

		ifASTNode->addChild(condNode);
		ifASTNode->addChild(thenStmtLstNode);
		ifASTNode->addChild(elseStmtLstNode);

		vector<Pattern> expectedPattern{ Pattern::createWhilePattern(-1, "y"),
										 Pattern::createAssignPattern(-1, " x ", " x 5 x + + "),
										 Pattern::createIfPattern(-1, "x y") };

		test(ifASTNode, expectedPattern);
	}
}