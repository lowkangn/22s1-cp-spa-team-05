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
          	if (!check) {
				cout << e.getLhs() << endl;
				cout << e.getRhs().size() << endl;
			}
			REQUIRE(check);
		}
	};

	SECTION("Test Assign") {
		vector<Pattern> expectedPattern{ Pattern::createAssignPattern(1, " x ", " x 1 + ") };

		Token assignToken = Token::createEqualsToken();
		Token xToken = Token::createNameOrKeywordToken("x");
		Token constantToken = Token::createIntegerToken("1");
		Token plusToken = Token::createPlusToken();

		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> x1Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> x2Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));
		shared_ptr<ASTNode> expressionNode(new ExpressionASTNode(plusToken));

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

		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> x1Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> x2Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));
		shared_ptr<ASTNode> expressionNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> expressionTwoNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> bracketNode(new BracketsASTNode(Token::getPlaceHolderToken()));

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
		shared_ptr<ASTNode> expectedASTNode(new IfASTNode(ifToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create then stmtlst node
		shared_ptr<ASTNode> stmtLstOneNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readOneNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		// Create else stmtlst node
		shared_ptr<ASTNode> stmtLstTwoNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readTwoNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> yTwoNode(new VariableASTNode(variableY));

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
		shared_ptr<ASTNode> expectedASTNode(new WhileASTNode(whileToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

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
			REQUIRE(extractedPattern[i].equals(expectedResult[i]));
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
		shared_ptr<ASTNode> programASTNode(new ProgramASTNode(programToken));

		// Create procedure
		shared_ptr<ASTNode> mainASTNode(new ProgramASTNode(mainToken));

		programASTNode->addChild(mainASTNode);

		// Procedure stmtlst
		shared_ptr<ASTNode> mainStmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		mainASTNode->addChild(mainStmtLstNode);


		// Create while node
		shared_ptr<ASTNode> ifASTNode(new IfASTNode(ifToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create then stmtlst node
		shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// create while node
		shared_ptr<ASTNode> whileASTNode(new WhileASTNode(whileToken));

		thenStmtLstNode->addChild(whileASTNode);

		// create while condition
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));
		shared_ptr<ASTNode> whileYOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> whileCondNode(new ExpressionASTNode(equalityToken));

		whileCondNode->addChild(whileYOneNode);
		whileCondNode->addChild(constantNode);

		whileASTNode->addChild(whileCondNode);

		// create while stmtlst node
		shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		whileStmtLstNode->addChild(readNode);
		whileStmtLstNode->addChild(xTwoNode);

		whileASTNode->addChild(whileStmtLstNode);

		// create else stmtlst node
		shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// create assign node
		Token assignToken = Token::createEqualsToken();
		Token plusToken = Token::createPlusToken();

		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> x1Node(new VariableASTNode(variableX));
		shared_ptr<ASTNode> x2Node(new VariableASTNode(variableX));
		shared_ptr<ASTNode> assignConstantNode(new ConstantValueASTNode(constantToken));
		shared_ptr<ASTNode> expressionNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> expressionTwoNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> bracketNode(new BracketsASTNode(Token::getPlaceHolderToken()));

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