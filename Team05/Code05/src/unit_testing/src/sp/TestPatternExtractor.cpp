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

TEST_CASE("PatternExtractor: infix to postfix") {
	auto test = [](string infixString, string expectedString) {
		// Given
		PatternExtractor extractor = PatternExtractor();

		// When
		string convertedString = extractor.convertInfixToPostFix(infixString);

		// Then
		REQUIRE(convertedString == expectedString);
	};

	test("A+B*C+D", "ABC*+D+");
	test("x*y/(5*z)+2", "xy*5z*/2+");
	test("x*y/(5*z)+2%3", "xy*5z*/23%+");
	test("y+5%3", "y53%+");
}


TEST_CASE("PatternExtractor: test handleAssign") {
	auto test = [](shared_ptr<ASTNode> ast, vector<Pattern> expectedResult) {
		// Given
		PatternExtractor extractor = PatternExtractor();

		// When
		vector<Pattern> extractedPattern = extractor.handleAssign(ast);

		// Then
		for (int i = 0; i < extractedPattern.size(); i++) {
			REQUIRE(extractedPattern[i].equals(expectedResult[i]));
		}
	};

	SECTION("Test Assign") {
		vector<Pattern> expectedPattern{ Pattern::createAssignPattern(1, "x", "x1+") };

		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token constantToken = Token{ "1", TokenType::INTEGER };
		Token plusToken = Token{ "+", TokenType::OPERATOR };

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
		vector<Pattern> expectedPattern{ Pattern::createAssignPattern(1, "x", "x1x++") };

		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token constantToken = Token{ "1", TokenType::INTEGER };
		Token plusToken = Token{ "+", TokenType::OPERATOR };

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
		vector<Pattern> expectedPattern{ Pattern::createIfPattern(1, IF_KEYWORD, "x y") };

		Token ifToken = Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token variableX = Token("x", TokenType::NAME_OR_KEYWORD);
		Token greaterThan = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
		Token variableY = Token("y", TokenType::NAME_OR_KEYWORD);
		Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token semiColonToken = Token(SEMI_COLON, TokenType::DELIMITER);

		// Create if node
		shared_ptr<ASTNode> expectedASTNode(new IfASTNode(ifToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create then stmtlst node
		shared_ptr<ASTNode> stmtLstOneNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

		shared_ptr<ASTNode> readOneNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		// Create else stmtlst node
		shared_ptr<ASTNode> stmtLstTwoNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

		shared_ptr<ASTNode> readTwoNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
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
		vector<Pattern> expectedPattern{ Pattern::createWhilePattern(1, WHILE_KEYWORD, "x y") };

		// while (x > y) then { read x; } else  { read y;}
		Token whileToken = Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token variableX = Token("x", TokenType::NAME_OR_KEYWORD);
		Token greaterThan = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
		Token variableY = Token("y", TokenType::NAME_OR_KEYWORD);
		Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token semiColonToken = Token(SEMI_COLON, TokenType::DELIMITER);

		// Create while node
		shared_ptr<ASTNode> expectedASTNode(new WhileASTNode(whileToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

		shared_ptr<ASTNode> readNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
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
		Token whileToken = Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token variableX = Token("x", TokenType::NAME_OR_KEYWORD);
		Token greaterThan = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
		Token equalityToken = Token(EQUALITY_OPERATOR, TokenType::OPERATOR);
		Token variableY = Token("y", TokenType::NAME_OR_KEYWORD);
		Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token programToken = Token(PROGRAM_KEYWORD, TokenType::INVALID);
		Token ifToken = Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD);
		Token mainToken = Token("main", TokenType::NAME_OR_KEYWORD);
		Token constantToken = Token("5", TokenType::INTEGER);

		// Create program
		shared_ptr<ASTNode> programASTNode(new ProgramASTNode(programToken));

		// Create procedure
		shared_ptr<ASTNode> mainASTNode(new ProgramASTNode(mainToken));

		programASTNode->addChild(mainASTNode);

		// Procedure stmtlst
		shared_ptr<ASTNode> mainStmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

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
		shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

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
		shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

		shared_ptr<ASTNode> readNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		whileStmtLstNode->addChild(readNode);
		whileStmtLstNode->addChild(xTwoNode);

		whileASTNode->addChild(whileStmtLstNode);

		// create else stmtlst node
		shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

		// create assign node
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token plusToken = Token{ "+", TokenType::OPERATOR };

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

		vector<Pattern> expectedPattern{ Pattern::createWhilePattern(-1, WHILE_KEYWORD, "y"), 
										 Pattern::createAssignPattern(-1, "x", "x5x++"), 
										 Pattern::createIfPattern(-1, IF_KEYWORD, "x y") };

		test(ifASTNode, expectedPattern);
	}
}