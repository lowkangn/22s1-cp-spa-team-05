#include "catch.hpp"
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/design_extractor/UsesExtractor.cpp>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>

using namespace std;

TEST_CASE("UsesExtractor: test handleAssign") {


	auto handleAssign = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleAssign(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};
	
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token addToken = Token::createPlusToken();
	Token constToken = Token::createIntegerToken("1");
	Token assignToken = Token::createEqualsToken();

	Entity assignEntity = Entity::createAssignEntity(1);
	Entity xEntity = Entity::createVariableEntity(1, xToken);
	Entity yEntity = Entity::createVariableEntity(1, yToken);
	Entity zEntity = Entity::createVariableEntity(1, zToken);

	SECTION("Only one variable on rhs") {

		// x = x + 1
		shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();;
		shared_ptr<ASTNode> addNode = ExpressionASTNode::createExpressionNode(addToken);
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constNode = ConstantValueASTNode::createConstantNode(constToken);

		addNode->setLineNumber(1);
		assignNode->setLineNumber(1);
		xNode->setLineNumber(1);
		constNode->setLineNumber(1);

		assignNode->addChild(xNode);
		assignNode->addChild(addNode);

		addNode->addChild(xNode);
		addNode->addChild(constNode);

		Relationship usesX = Relationship::createUsesRelationship(assignEntity, xEntity);

		vector<Relationship> expectedResult = vector<Relationship>{ usesX };

		handleAssign(assignNode, expectedResult);
	}

	SECTION("More than one variable on rhs") {

		// x = x + y + z
		shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();;
		shared_ptr<ASTNode> addNode1 = ExpressionASTNode::createExpressionNode(addToken);
		shared_ptr<ASTNode> addNode2 = ExpressionASTNode::createExpressionNode(addToken);
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> zNode = VariableASTNode::createVariableNode(zToken);

		addNode1->setLineNumber(1);
		addNode2->setLineNumber(1);
		assignNode->setLineNumber(1);
		xNode->setLineNumber(1);
		yNode->setLineNumber(1);
		zNode->setLineNumber(1);

		assignNode->addChild(xNode);
		assignNode->addChild(addNode2);

		addNode2->addChild(addNode1);
		addNode2->addChild(zNode);

		addNode1->addChild(xNode);
		addNode1->addChild(yNode);

		Relationship usesX = Relationship::createUsesRelationship(assignEntity, xEntity);
		Relationship usesY = Relationship::createUsesRelationship(assignEntity, yEntity);
		Relationship usesZ = Relationship::createUsesRelationship(assignEntity, zEntity);

		vector<Relationship> expectedResult = vector<Relationship>{ usesX, usesY, usesZ };

		handleAssign(assignNode, expectedResult);
	}

	SECTION("No variables on rhs") {
		// x = 1
		shared_ptr<ASTNode> assignNode= AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constNode = ConstantValueASTNode::createConstantNode(constToken);

		assignNode->setLineNumber(1);
		xNode->setLineNumber(1);
		constNode->setLineNumber(1);

		assignNode->addChild(xNode);
		assignNode->addChild(constNode);

		vector<Relationship> expectedResult = vector<Relationship>();

		handleAssign(assignNode, expectedResult);
	}
}

TEST_CASE("UsesExtractor: test handlePrint") {


	auto handlePrint = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handlePrint(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

	Token printToken = Token::createPrintToken();
	Token xToken = Token::createNameOrKeywordToken("x");

	Entity printEntity = Entity::createPrintEntity(1);
	Entity xEntity = Entity::createVariableEntity(1, xToken);

	// print x
	shared_ptr<ASTNode> printNode = PrintASTNode::createPrintNode();	
	shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(xToken);

	printNode->setLineNumber(1);
	xNode->setLineNumber(1);

	printNode->addChild(xNode);

	Relationship usesX = Relationship::createUsesRelationship(printEntity, xEntity);

	vector<Relationship> expectedResult = vector<Relationship>{ usesX };

	handlePrint(printNode, expectedResult);
}

TEST_CASE("UsesExtractor: test handleProcedure") {


	auto handleProcedure = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleProcedure(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(1, leftToken);

	/*
		procedure main {
			x = y;
			print z;
		}
	*/
	// Creating tokens
	Token mainToken = Token::createNameOrKeywordToken("");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token printToken = Token::createPrintToken();
	Token assignToken = Token::createEqualsToken();
	Token stmtLst = Token::createPlaceholderToken();
	Token addToken = Token::createPlusToken();

	// Creating nodes
	shared_ptr<ASTNode> printNode = PrintASTNode::createPrintNode();
	shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(mainToken);

	shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();

	shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
	shared_ptr<ASTNode> x = VariableASTNode::createVariableNode(xToken);
	shared_ptr<ASTNode> y = VariableASTNode::createVariableNode(yToken);
	shared_ptr<ASTNode> z = VariableASTNode::createVariableNode(zToken);

	x->setLineNumber(1);
	y->setLineNumber(1);
	z->setLineNumber(2);
	assignNode->setLineNumber(1);
	printNode->setLineNumber(2);

	procedureNode->addChild(stmtLstNode);

	stmtLstNode->addChild(assignNode);
	stmtLstNode->addChild(printNode);

	assignNode->addChild(x);
	assignNode->addChild(y);

	printNode->addChild(z);

	// Creating Relationship
	Entity procedureEntity = Entity::createProcedureEntity(mainToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity zEntity = Entity::createVariableEntity(z->getLineNumber(), zToken);

	Relationship procedureYRelation = Relationship::createUsesRelationship(procedureEntity, yEntity);
	Relationship procedureZRelation = Relationship::createUsesRelationship(procedureEntity, zEntity);

	vector<Relationship> expectedResult = vector<Relationship>{ procedureYRelation, procedureZRelation };

	handleProcedure(procedureNode, expectedResult);
}

TEST_CASE("UsesExtractor: test handleWhile") {
	auto handleWhile = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleWhile(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

	/*
		while (x != y) {
			print z;
			y = y + 1;
		}
	*/
	// Creating tokens
	Token whileToken = Token::createWhileToken();
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token printToken = Token::createPrintToken();
	Token assignToken = Token::createEqualsToken();
	Token stmtLst = Token::createPlaceholderToken();
	Token constToken = Token::createIntegerToken("1");
	Token addToken = Token::createPlusToken();
	Token notEqualToken = Token::createNotEqualsToken();

	// Creating nodes
	shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();	
	shared_ptr<ASTNode> conditionNode = ExpressionASTNode::createExpressionNode(notEqualToken);
	shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
	shared_ptr<ASTNode> printNode = PrintASTNode::createPrintNode();
	shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();
	shared_ptr<ASTNode> addNode = ExpressionASTNode::createExpressionNode(notEqualToken);
	shared_ptr<ASTNode> constNode = ConstantValueASTNode::createConstantNode(constToken);

	shared_ptr<ASTNode> x = VariableASTNode::createVariableNode(xToken);
	shared_ptr<ASTNode> y = VariableASTNode::createVariableNode(yToken);
	shared_ptr<ASTNode> z = VariableASTNode::createVariableNode(zToken);

	x->setLineNumber(1);
	y->setLineNumber(1);
	z->setLineNumber(2);
	whileNode->setLineNumber(1);
	conditionNode->setLineNumber(1);
	printNode->setLineNumber(2);
	assignNode->setLineNumber(3);
	addNode->setLineNumber(3);
	constNode->setLineNumber(3);

	conditionNode->addChild(x);
	conditionNode->addChild(y);

	whileNode->addChild(conditionNode);
	whileNode->addChild(stmtLstNode);

	stmtLstNode->addChild(printNode);
	stmtLstNode->addChild(assignNode);

	addNode->addChild(y);
	addNode->addChild(constNode);

	assignNode->addChild(y);
	assignNode->addChild(addNode);

	printNode->addChild(z);

	// Creating Relationship
	Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity zEntity = Entity::createVariableEntity(z->getLineNumber(), zToken);

	Relationship whileXRelation = Relationship::createUsesRelationship(whileEntity, xEntity);
	Relationship whileYRelation = Relationship::createUsesRelationship(whileEntity, yEntity);
	Relationship whileZRelation = Relationship::createUsesRelationship(whileEntity, zEntity);


	vector<Relationship> expectedResult = vector<Relationship>{ whileXRelation, whileYRelation, whileZRelation, whileYRelation };

	handleWhile(whileNode, expectedResult);
}

TEST_CASE("UsesExtractor: test handleIf") {
	auto handleIf = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleIf(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

	/*
		1. if (x == 0) then {
		2. 	   x = y;
		}  else {
		3.     y = x;
		}
	*/
	// Creating tokens
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constToken = Token::createIntegerToken("0");
	Token ifToken = Token::createIfToken();
	Token equalityToken = Token::createEqualityToken();
	Token assignToken = Token::createEqualsToken();
	Token stmtLst = Token::createPlaceholderToken();

	// Creating nodes
	shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();

	shared_ptr<ASTNode> condNode = ExpressionASTNode::createExpressionNode(equalityToken);
	shared_ptr<ASTNode> xCond = VariableASTNode::createVariableNode(xToken);
	shared_ptr<ASTNode> constNode = ConstantValueASTNode::createConstantNode(constToken);

	shared_ptr<ASTNode> assignXNode = AssignASTNode::createAssignNode();
	shared_ptr<ASTNode> assignYNode = AssignASTNode::createAssignNode();

	shared_ptr<ASTNode> thenStmtLstNode = StatementListASTNode::createStatementListNode();	
	shared_ptr<ASTNode> elseStmtLstNode = StatementListASTNode::createStatementListNode();
	shared_ptr<ASTNode> xThen = VariableASTNode::createVariableNode(xToken);
	shared_ptr<ASTNode> yThen = VariableASTNode::createVariableNode(yToken);

	shared_ptr<ASTNode> yElse  = VariableASTNode::createVariableNode(yToken);
	shared_ptr<ASTNode> xElse = VariableASTNode::createVariableNode(xToken);


	ifNode->setLineNumber(1);
	xCond->setLineNumber(1);
	condNode->setLineNumber(1);
	constNode->setLineNumber(1);
	xThen->setLineNumber(2);
	yThen->setLineNumber(2);
	assignXNode->setLineNumber(2);
	yElse->setLineNumber(3);
	xElse->setLineNumber(3);
	assignYNode->setLineNumber(3);

	ifNode->addChild(condNode);
	ifNode->addChild(thenStmtLstNode);
	ifNode->addChild(elseStmtLstNode);

	condNode->addChild(xCond);
	condNode->addChild(constNode);

	thenStmtLstNode->addChild(assignXNode);
	assignXNode->addChild(xThen);
	assignXNode->addChild(yThen);

	elseStmtLstNode->addChild(assignYNode);
	assignYNode->addChild(yElse);
	assignYNode->addChild(xElse);

	// Creating Relationships
	Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());
	Entity xCondEntity = Entity::createVariableEntity(xCond->getLineNumber(), xToken);
	Entity yThenEntity = Entity::createVariableEntity(yThen->getLineNumber(), yToken);
	Entity xElseEntity = Entity::createVariableEntity(xElse->getLineNumber(), xToken);

	Relationship ifCondXRelation = Relationship::createUsesRelationship(ifEntity, xCondEntity);
	Relationship ifThenYRelation = Relationship::createUsesRelationship(ifEntity, yThenEntity);
	Relationship ifElseXRelation = Relationship::createUsesRelationship(ifEntity, xElseEntity);


	vector<Relationship> expectedResult = vector<Relationship>{ ifCondXRelation, ifThenYRelation, ifElseXRelation };

	handleIf(ifNode, expectedResult);
}


TEST_CASE("UsesExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> rootNode, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.extract(rootNode);
		
		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			bool isInExpectedRelationships = false;
			for (int j = 0; j < expectedResult.size(); j++) {
				if (extractedResult[i].equals(expectedResult[j])) {
					isInExpectedRelationships = true;
					break;
				}
			}
			
			REQUIRE(isInExpectedRelationships);
		}
	};
	SECTION("Procedure with ifs and whiles") {
		/*
			procedure main {
				1. x = 3;
				2. read y;
				3. while ( x != 0 ) {
				4.     x = x - 1
				5.     print y;
				}
				6. if (y > 5) then {
				7.     x = y;
				} else {
				8.     y = x;
				}
			}
		*/
		// Creating tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token assignToken = Token::createEqualsToken();
		Token notEqualToken = Token::createNotEqualsToken();
		Token greaterToken = Token::createGreaterThanToken();
		Token minusToken = Token::createMinusToken();
		Token constThreeToken = Token::createIntegerToken("3");
		Token constZeroToken = Token::createIntegerToken("0");
		Token constOneToken = Token::createIntegerToken("1");
		Token constFiveToken = Token::createIntegerToken("5");
		Token readToken = Token::createReadToken();
		Token whileToken = Token::createWhileToken();
		Token printToken = Token::createPrintToken();
		Token ifToken = Token::createIfToken();
		Token stmtListToken = Token::createPlaceholderToken();

		// Creating AST nodes
		shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(mainToken);

		// Line 1 (x = 3)
		shared_ptr<ASTNode> mainStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> x1Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> assign1Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> constThreeNode = ConstantValueASTNode::createConstantNode(constThreeToken);

		x1Node->setLineNumber(1);
		assign1Node->setLineNumber(1);
		constThreeNode->setLineNumber(1);

		// Line 2 (read y)
		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();
		shared_ptr<ASTNode> y2Node = VariableASTNode::createVariableNode(yToken);

		readNode->setLineNumber(2);
		y2Node->setLineNumber(2);

		// Line 3 (while ( x != 0 ))
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();
		shared_ptr<ASTNode> x3Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constZeroNode = ConstantValueASTNode::createConstantNode(constZeroToken);
		shared_ptr<ASTNode> notEqualNode = ExpressionASTNode::createExpressionNode(notEqualToken);

		whileNode->setLineNumber(3);
		x3Node->setLineNumber(3);
		constZeroNode->setLineNumber(3);
		notEqualNode->setLineNumber(3);

		// Line 4 (x = x - 1)
		shared_ptr<ASTNode> whileStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> x4LhsNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> x4RhsNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> assign4Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> minusNode = ExpressionASTNode::createExpressionNode(minusToken);
		shared_ptr<ASTNode> constOneNode = ConstantValueASTNode::createConstantNode(constOneToken);

		x4LhsNode->setLineNumber(4);
		x4RhsNode->setLineNumber(4);
		assign4Node->setLineNumber(4);
		minusNode->setLineNumber(4);
		constOneNode->setLineNumber(4);

		// Line 5 (print y)
		shared_ptr<ASTNode> printNode = PrintASTNode::createPrintNode();
		shared_ptr<ASTNode> y5Node = VariableASTNode::createVariableNode(yToken);

		printNode->setLineNumber(5);
		y5Node->setLineNumber(5);

		// Line 6 (if (y > 5) then)
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> y6Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> greaterNode = ExpressionASTNode::createExpressionNode(greaterToken);
		shared_ptr<ASTNode> constFiveNode = ConstantValueASTNode::createConstantNode(constFiveToken);

		ifNode->setLineNumber(6);
		y6Node->setLineNumber(6);
		greaterNode->setLineNumber(6);
		constFiveNode->setLineNumber(6);

		// Line 7 (x = y)
		shared_ptr<ASTNode> thenStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> assign7Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> x7Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> y7Node = VariableASTNode::createVariableNode(yToken);

		assign7Node->setLineNumber(7);
		x7Node->setLineNumber(7);
		y7Node->setLineNumber(7);

		// Line 8 (y = x)
		shared_ptr<ASTNode> elseStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> assign8Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> y8Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> x8Node = VariableASTNode::createVariableNode(xToken);

		assign8Node->setLineNumber(8);
		y8Node->setLineNumber(8);
		x8Node->setLineNumber(8);

		procedureNode->addChild(mainStmtList);

		mainStmtList->addChild(assign1Node);
		mainStmtList->addChild(readNode);
		mainStmtList->addChild(whileNode);
		mainStmtList->addChild(ifNode);

		assign1Node->addChild(x1Node);
		assign1Node->addChild(constThreeNode);

		readNode->addChild(y2Node);

		whileNode->addChild(notEqualNode);
		whileNode->addChild(whileStmtList);

		notEqualNode->addChild(x3Node);
		notEqualNode->addChild(constZeroNode);

		whileStmtList->addChild(assign4Node);
		whileStmtList->addChild(printNode);

		assign4Node->addChild(x4LhsNode);
		assign4Node->addChild(minusNode);

		minusNode->addChild(x4RhsNode);
		minusNode->addChild(constOneNode);

		printNode->addChild(y5Node);

		ifNode->addChild(greaterNode);
		ifNode->addChild(thenStmtList);
		ifNode->addChild(elseStmtList);

		greaterNode->addChild(y6Node);
		greaterNode->addChild(constFiveNode);

		thenStmtList->addChild(assign7Node);

		assign7Node->addChild(x7Node);
		assign7Node->addChild(y7Node);

		elseStmtList->addChild(assign8Node);

		assign8Node->addChild(y8Node);
		assign8Node->addChild(x8Node);

		// Creating Entities
		Entity procedureEntity = Entity::createProcedureEntity(mainToken);

		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());
		Entity y2Entity = Entity::createVariableEntity(y2Node->getLineNumber(), yToken);

		Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());
		Entity xCond = Entity::createVariableEntity(x3Node->getLineNumber(), xToken);

		Entity assign4Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
		Entity x4Entity = Entity::createVariableEntity(x4RhsNode->getLineNumber(), xToken);
		Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());
		Entity y5Entity = Entity::createVariableEntity(y5Node->getLineNumber(), yToken);

		Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());
		Entity yCondEntity = Entity::createVariableEntity(y6Node->getLineNumber(), yToken);

		Entity assign7Entity = Entity::createAssignEntity(assign7Node->getLineNumber());
		Entity y7Entity = Entity::createVariableEntity(y7Node->getLineNumber(), yToken);

		Entity assign8Entity = Entity::createAssignEntity(assign8Node->getLineNumber());
		Entity x8Entity = Entity::createVariableEntity(x8Node->getLineNumber(), xToken);

		// Creating relationships
		Relationship mainUsesXCond = Relationship::createUsesRelationship(procedureEntity, xCond);
		Relationship mainUsesX4 = Relationship::createUsesRelationship(procedureEntity, x4Entity);
		Relationship mainUsesY5 = Relationship::createUsesRelationship(procedureEntity, y5Entity);
		Relationship mainUsesYCond = Relationship::createUsesRelationship(procedureEntity, yCondEntity);
		Relationship mainUsesY7 = Relationship::createUsesRelationship(procedureEntity, y7Entity);
		Relationship mainUsesX8 = Relationship::createUsesRelationship(procedureEntity, x8Entity);

		Relationship whileUsesXCond = Relationship::createUsesRelationship(whileEntity, xCond);
		Relationship whileUsesX4 = Relationship::createUsesRelationship(whileEntity, x4Entity);
		Relationship whileUsesY5 = Relationship::createUsesRelationship(whileEntity, y5Entity);

		Relationship assign4UsesX4 = Relationship::createUsesRelationship(assign4Entity, x4Entity);
		Relationship printUsesY5 = Relationship::createUsesRelationship(printEntity, y5Entity);

		Relationship ifUsesYCond = Relationship::createUsesRelationship(ifEntity, yCondEntity);
		Relationship ifUsesY7 = Relationship::createUsesRelationship(ifEntity, y7Entity);
		Relationship ifUsesX8 = Relationship::createUsesRelationship(ifEntity, x8Entity);

		Relationship assign7UsesY7 = Relationship::createUsesRelationship(assign7Entity, y7Entity);
		Relationship assign8UsesX8 = Relationship::createUsesRelationship(assign8Entity, x8Entity);

		vector<Relationship> expectedResult = vector<Relationship>{ mainUsesXCond, mainUsesX4, mainUsesY5, mainUsesYCond, mainUsesY7, mainUsesX8,
				whileUsesXCond, whileUsesX4, whileUsesY5, assign4UsesX4, printUsesY5, ifUsesYCond, ifUsesY7, ifUsesX8, assign7UsesY7, assign8UsesX8 };


		testExtract(procedureNode, expectedResult);
	}

	SECTION("Multi-procedure program with call statements") {
		/*
		procedure main {
			1. x = 3;
			2. read y;
			3. while ( x != 0 ) {
			4.     x = x - 1
			5.     print y;
			}
			6. if (y > 5) then {
			7.     x = y;
			} else {
			8.     y = x;
			}
			9. call alpha;
		}

		procedure alpha {
			10. print x; 
			11. y = z;
			12. z = 7;
			13. call beta;
		}

		procedure beta {
			14. a = b; // changed
		}
		*/

		// Creating tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token alphaToken = Token::createNameOrKeywordToken("alpha");
		Token betaToken = Token::createNameOrKeywordToken("beta");


		Token callToken = Token::createCallToken();

		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token zToken = Token::createNameOrKeywordToken("z");
		Token aToken = Token::createNameOrKeywordToken("a");
		Token bToken = Token::createNameOrKeywordToken("b");
		Token assignToken = Token::createEqualsToken();
		Token notEqualToken = Token::createNotEqualsToken();
		Token greaterToken = Token::createGreaterThanToken();
		Token minusToken = Token::createMinusToken();
		Token constThreeToken = Token::createIntegerToken("3");
		Token constZeroToken = Token::createIntegerToken("0");
		Token constOneToken = Token::createIntegerToken("1");
		Token constFiveToken = Token::createIntegerToken("5");
		Token constSixToken = Token::createIntegerToken("6");
		Token constSevenToken = Token::createIntegerToken("7");
		Token constEightToken = Token::createIntegerToken("8");

		Token readToken = Token::createReadToken();
		Token whileToken = Token::createWhileToken();
		Token printToken = Token::createPrintToken();
		Token ifToken = Token::createIfToken();
		Token stmtListToken = Token::createPlaceholderToken();

		// Creating AST nodes

		// Root level
		shared_ptr<ASTNode> rootProgramNode = ProgramASTNode::createProgramNode();

		shared_ptr<ASTNode> procedureMainNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> procedureAlphaNode = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> procedureBetaNode = ProcedureASTNode::createProcedureNode(betaToken);


		rootProgramNode->addChild(procedureMainNode);
		rootProgramNode->addChild(procedureAlphaNode);
		rootProgramNode->addChild(procedureBetaNode);


		// Main

		// Line 1 (x = 3)
		shared_ptr<ASTNode> mainStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> x1Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> assign1Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> constThreeNode = ConstantValueASTNode::createConstantNode(constThreeToken);

		x1Node->setLineNumber(1);
		assign1Node->setLineNumber(1);
		constThreeNode->setLineNumber(1);

		// Line 2 (read y)
		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();
		shared_ptr<ASTNode> y2Node = VariableASTNode::createVariableNode(yToken);

		readNode->setLineNumber(2);
		y2Node->setLineNumber(2);

		// Line 3 (while ( x != 0 ))
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();
		shared_ptr<ASTNode> x3Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constZeroNode = ConstantValueASTNode::createConstantNode(constZeroToken);
		shared_ptr<ASTNode> notEqualNode = ExpressionASTNode::createExpressionNode(notEqualToken);

		whileNode->setLineNumber(3);
		x3Node->setLineNumber(3);
		constZeroNode->setLineNumber(3);
		notEqualNode->setLineNumber(3);

		// Line 4 (x = x - 1)
		shared_ptr<ASTNode> whileStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> x4LhsNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> x4RhsNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> assign4Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> minusNode = ExpressionASTNode::createExpressionNode(minusToken);
		shared_ptr<ASTNode> constOneNode = ConstantValueASTNode::createConstantNode(constOneToken);

		x4LhsNode->setLineNumber(4);
		x4RhsNode->setLineNumber(4);
		assign4Node->setLineNumber(4);
		minusNode->setLineNumber(4);
		constOneNode->setLineNumber(4);

		// Line 5 (print y)
		shared_ptr<ASTNode> printNode = PrintASTNode::createPrintNode();
		shared_ptr<ASTNode> y5Node = VariableASTNode::createVariableNode(yToken);

		printNode->setLineNumber(5);
		y5Node->setLineNumber(5);

		// Line 6 (if (y > 5) then)
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> y6Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> greaterNode = ExpressionASTNode::createExpressionNode(greaterToken);
		shared_ptr<ASTNode> constFiveNode = ConstantValueASTNode::createConstantNode(constFiveToken);

		ifNode->setLineNumber(6);
		y6Node->setLineNumber(6);
		greaterNode->setLineNumber(6);
		constFiveNode->setLineNumber(6);

		// Line 7 (x = y)
		shared_ptr<ASTNode> thenStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> assign7Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> x7Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> y7Node = VariableASTNode::createVariableNode(yToken);

		assign7Node->setLineNumber(7);
		x7Node->setLineNumber(7);
		y7Node->setLineNumber(7);

		// Line 8 (y = x)
		shared_ptr<ASTNode> elseStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> assign8Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> y8Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> x8Node = VariableASTNode::createVariableNode(xToken);

		assign8Node->setLineNumber(8);
		y8Node->setLineNumber(8);
		x8Node->setLineNumber(8);

		// Line 9 (call alpha)
		shared_ptr<ASTNode> callAlpha = CallASTNode::createCallNode();
		shared_ptr<ASTNode> calledAlphaProc = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> calledAlphaStmtList = StatementListASTNode::createStatementListNode();

		callAlpha->setLineNumber(9);


		procedureMainNode->addChild(mainStmtList);

		mainStmtList->addChild(assign1Node);
		mainStmtList->addChild(readNode);
		mainStmtList->addChild(whileNode);
		mainStmtList->addChild(ifNode);
		mainStmtList->addChild(callAlpha);

		assign1Node->addChild(x1Node);
		assign1Node->addChild(constThreeNode);

		readNode->addChild(y2Node);

		whileNode->addChild(notEqualNode);
		whileNode->addChild(whileStmtList);

		notEqualNode->addChild(x3Node);
		notEqualNode->addChild(constZeroNode);

		whileStmtList->addChild(assign4Node);
		whileStmtList->addChild(printNode);

		assign4Node->addChild(x4LhsNode);
		assign4Node->addChild(minusNode);

		minusNode->addChild(x4RhsNode);
		minusNode->addChild(constOneNode);

		printNode->addChild(y5Node);

		ifNode->addChild(greaterNode);
		ifNode->addChild(thenStmtList);
		ifNode->addChild(elseStmtList);

		greaterNode->addChild(y6Node);
		greaterNode->addChild(constFiveNode);

		thenStmtList->addChild(assign7Node);

		assign7Node->addChild(x7Node);
		assign7Node->addChild(y7Node);

		elseStmtList->addChild(assign8Node);

		assign8Node->addChild(y8Node);
		assign8Node->addChild(x8Node);

		callAlpha->addChild(calledAlphaProc);
		calledAlphaProc->addChild(calledAlphaStmtList);

		// Alpha
		// Line 10 (print x)
		shared_ptr<ASTNode> alphaStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> printXNode = PrintASTNode::createPrintNode();
		shared_ptr<ASTNode> x10Node = VariableASTNode::createVariableNode(xToken);

		printXNode->setLineNumber(10);
		x10Node->setLineNumber(10);

		// Line 11 (y = z)
		shared_ptr<ASTNode> y11Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> assign11Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> z11Node = VariableASTNode::createVariableNode(zToken);

		y11Node->setLineNumber(11);
		assign11Node->setLineNumber(11);
		z11Node->setLineNumber(11);

		// Line 12 (z = 7)
		shared_ptr<ASTNode> z12Node = VariableASTNode::createVariableNode(zToken);
		shared_ptr<ASTNode> assign12Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> constSevenNode = ConstantValueASTNode::createConstantNode(constSevenToken);

		z12Node->setLineNumber(11);
		assign12Node->setLineNumber(11);
		constSevenNode->setLineNumber(11);

		// Line 13 (call beta)
		shared_ptr<ASTNode> callBeta = CallASTNode::createCallNode();
		shared_ptr<ASTNode> calledBetaProc = ProcedureASTNode::createProcedureNode(betaToken);
		shared_ptr<ASTNode> calledBetaStmtList = StatementListASTNode::createStatementListNode();

		callBeta->setLineNumber(13);

		procedureAlphaNode->addChild(alphaStmtList);

		alphaStmtList->addChild(printXNode);
		alphaStmtList->addChild(assign11Node);
		alphaStmtList->addChild(assign12Node);
		alphaStmtList->addChild(callBeta);

		printXNode->addChild(x10Node);

		assign11Node->addChild(y11Node);
		assign11Node->addChild(z11Node);

		assign12Node->addChild(z12Node);
		assign12Node->addChild(constSevenNode);

		callBeta->addChild(calledBetaProc);
		calledBetaProc->addChild(calledBetaStmtList);


		// Beta
		// Line 14 (a = b)
		shared_ptr<ASTNode> betaStmtList = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> a14Node = VariableASTNode::createVariableNode(aToken);
		shared_ptr<ASTNode> assign14Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> b14Node = VariableASTNode::createVariableNode(bToken);

		a14Node->setLineNumber(14);
		assign14Node->setLineNumber(14);
		b14Node->setLineNumber(14);

		procedureBetaNode->addChild(betaStmtList);

		betaStmtList->addChild(assign14Node);

		assign14Node->addChild(a14Node);
		assign14Node->addChild(b14Node);

		// Creating entities

		// Main
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);

		Entity assign1Entity = Entity::createAssignEntity(assign1Node->getLineNumber());
		Entity x1Entity = Entity::createVariableEntity(x1Node->getLineNumber(), xToken);
		Entity y2Entity = Entity::createVariableEntity(y2Node->getLineNumber(), yToken);
		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

		Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());
		Entity xCond = Entity::createVariableEntity(x3Node->getLineNumber(), xToken);

		Entity assign4Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
		Entity x4Entity = Entity::createVariableEntity(x4RhsNode->getLineNumber(), xToken);
		Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());
		Entity y5Entity = Entity::createVariableEntity(y5Node->getLineNumber(), yToken);

		Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());
		Entity yCondEntity = Entity::createVariableEntity(y6Node->getLineNumber(), yToken);


		Entity assign7Entity = Entity::createAssignEntity(assign7Node->getLineNumber());
		Entity y7Entity = Entity::createVariableEntity(y7Node->getLineNumber(), yToken);

		Entity assign8Entity = Entity::createAssignEntity(assign8Node->getLineNumber());
		Entity x8Entity = Entity::createVariableEntity(x8Node->getLineNumber(), xToken);


		Entity callAlphaEntity = Entity::createCallEntity(callAlpha->getLineNumber());

		// Alpha
		Entity procedureAlphaEntity = Entity::createProcedureEntity(alphaToken);
		Entity printXEntity = Entity::createPrintEntity(printXNode->getLineNumber());
		Entity assign11Entity = Entity::createAssignEntity(assign11Node->getLineNumber());
		Entity assign12Entity = Entity::createAssignEntity(assign12Node->getLineNumber());

		Entity x10Entity = Entity::createVariableEntity(x10Node->getLineNumber(), xToken);
		Entity z11Entity = Entity::createVariableEntity(z11Node->getLineNumber(), zToken);
		Entity z12Entity = Entity::createVariableEntity(z12Node->getLineNumber(), zToken);

		Entity callBetaEntity = Entity::createCallEntity(callBeta->getLineNumber());

		// Beta
		Entity procedureBetaEntity = Entity::createProcedureEntity(betaToken);
		Entity b14Entity = Entity::createVariableEntity(b14Node->getLineNumber(), bToken);
		Entity assign14Entity = Entity::createAssignEntity(assign14Node->getLineNumber());

		// Creating relationships
		Relationship mainUsesXCond = Relationship::createUsesRelationship(procedureMainEntity, xCond);
		Relationship mainUsesX4 = Relationship::createUsesRelationship(procedureMainEntity, x4Entity);
		Relationship mainUsesY5 = Relationship::createUsesRelationship(procedureMainEntity, y5Entity);
		Relationship mainUsesYCond = Relationship::createUsesRelationship(procedureMainEntity, yCondEntity);
		Relationship mainUsesY7 = Relationship::createUsesRelationship(procedureMainEntity, y7Entity);
		Relationship mainUsesX8 = Relationship::createUsesRelationship(procedureMainEntity, x8Entity);

		Relationship whileUsesXCond = Relationship::createUsesRelationship(whileEntity, xCond);
		Relationship whileUsesX4 = Relationship::createUsesRelationship(whileEntity, x4Entity);
		Relationship whileUsesY5 = Relationship::createUsesRelationship(whileEntity, y5Entity);

		Relationship assign4UsesX4 = Relationship::createUsesRelationship(assign4Entity, x4Entity);
		Relationship printUsesY5 = Relationship::createUsesRelationship(printEntity, y5Entity);

		Relationship ifUsesYCond = Relationship::createUsesRelationship(ifEntity, yCondEntity);
		Relationship ifUsesY7 = Relationship::createUsesRelationship(ifEntity, y7Entity);
		Relationship ifUsesX8 = Relationship::createUsesRelationship(ifEntity, x8Entity);

		Relationship assign7UsesY7 = Relationship::createUsesRelationship(assign7Entity, y7Entity);
		Relationship assign8UsesX8 = Relationship::createUsesRelationship(assign8Entity, x8Entity);


		// extras
		Relationship mainUsesX10 = Relationship::createUsesRelationship(procedureMainEntity, x10Entity);
		Relationship mainUsesZ11 = Relationship::createUsesRelationship(procedureMainEntity, z11Entity);
		Relationship mainUsesB14 = Relationship::createUsesRelationship(procedureMainEntity, b14Entity);

		Relationship callAlphaUsesX10 = Relationship::createUsesRelationship(callAlphaEntity, x10Entity);
		Relationship callAlphaUsesZ11 = Relationship::createUsesRelationship(callAlphaEntity, z11Entity);
		Relationship callAlphaUsesZ14 = Relationship::createUsesRelationship(callAlphaEntity, b14Entity);

		Relationship alphaUsesX10 = Relationship::createUsesRelationship(procedureAlphaEntity, x10Entity);
		Relationship alphaUsesZ11 = Relationship::createUsesRelationship(procedureAlphaEntity, z11Entity);
		Relationship alphaUsesB14 = Relationship::createUsesRelationship(procedureAlphaEntity, b14Entity);

		Relationship printUsesX10 = Relationship::createUsesRelationship(printXEntity, x10Entity);

		Relationship assign11UsesZ11 = Relationship::createUsesRelationship(assign11Entity, z11Entity);

		Relationship callBetaUsesB14 = Relationship::createUsesRelationship(callBetaEntity, b14Entity);

		Relationship betaUsesB14 = Relationship::createUsesRelationship(procedureBetaEntity, b14Entity);
		Relationship assign14UsesB14 = Relationship::createUsesRelationship(assign14Entity, b14Entity);


		vector<Relationship> expectedResult = vector<Relationship>{ mainUsesXCond, mainUsesX4, mainUsesY5, mainUsesYCond, mainUsesY7, mainUsesX8,
				whileUsesXCond, whileUsesX4, whileUsesY5, assign4UsesX4, printUsesY5, ifUsesYCond, ifUsesY7, ifUsesX8, assign7UsesY7, assign8UsesX8,
				mainUsesX10, mainUsesZ11, mainUsesB14, callAlphaUsesX10, callAlphaUsesZ11, callAlphaUsesZ14,
				alphaUsesX10, alphaUsesZ11, alphaUsesB14, printUsesX10, assign11UsesZ11, callBetaUsesB14,
				betaUsesB14, assign14UsesB14 };


		testExtract(rootProgramNode, expectedResult);
	}
}