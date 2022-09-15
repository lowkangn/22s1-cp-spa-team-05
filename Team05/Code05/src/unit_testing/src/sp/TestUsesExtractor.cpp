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
	
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token zToken = Token{ "z", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ PLUS_OPERATOR, TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };

	Entity assignEntity = Entity::createAssignEntity(1);
	Entity xEntity = Entity::createVariableEntity(1, xToken);
	Entity yEntity = Entity::createVariableEntity(1, yToken);
	Entity zEntity = Entity::createVariableEntity(1, zToken);

	SECTION("Only one variable on rhs") {

		// x = x + 1
		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> addNode(new ExpressionASTNode(addToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
		
		addNode->setLineNumber(1);
		assignNode->setLineNumber(1);
		xNode->setLineNumber(1);
		constNode->setLineNumber(1);

		assignNode->addChild(xNode);
		assignNode->addChild(addNode);

		addNode->addChild(xNode);
		addNode->addChild(constNode);

		Relationship usesX = Relationship{ assignEntity, xEntity, RelationshipType::USES };

		vector<Relationship> expectedResult = vector<Relationship>{ usesX };

		handleAssign(assignNode, expectedResult);
	}
	
	SECTION("More than one variable on rhs") {

		// x = x + y + z
		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> addNode1(new ExpressionASTNode(addToken));
		shared_ptr<ASTNode> addNode2(new ExpressionASTNode(addToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
		shared_ptr<ASTNode> zNode(new VariableASTNode(zToken));

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

		Relationship usesX = Relationship{ assignEntity, xEntity, RelationshipType::USES };
		Relationship usesY = Relationship{ assignEntity, yEntity, RelationshipType::USES };
		Relationship usesZ = Relationship{ assignEntity, zEntity, RelationshipType::USES };

		vector<Relationship> expectedResult = vector<Relationship>{ usesX, usesY, usesZ };

		handleAssign(assignNode, expectedResult);
	}

	SECTION("No variables on rhs") {
		// x = 1
		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

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

	Token printToken = { PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };

	Entity printEntity = Entity::createPrintEntity(1);
	Entity xEntity = Entity::createVariableEntity(1, xToken);

	// print x
	shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
	shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));

	printNode->setLineNumber(1);
	xNode->setLineNumber(1);

	printNode->addChild(xNode);

	Relationship usesX = Relationship{ printEntity, xEntity, RelationshipType::USES };

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

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(1, leftToken);

	/*
		procedure main {
			x = y;
			print z;
		}
	*/
	// Creating tokens
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token zToken = Token{ "z", TokenType::NAME_OR_KEYWORD };
	Token printToken = Token{ PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };
	Token addToken = Token{ PLUS_OPERATOR, TokenType::OPERATOR };

	// Creating nodes
	shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));
	shared_ptr<ASTNode> z(new VariableASTNode(zToken));

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
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity zEntity = Entity::createVariableEntity(z->getLineNumber(), zToken);
	Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());
	Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());

	Relationship procedureYRelation = Relationship{ procedureEntity, yEntity, RelationshipType::USES };
	Relationship procedureZRelation = Relationship{ procedureEntity, zEntity, RelationshipType::USES };
	Relationship assignRelation = Relationship{ assignEntity, yEntity, RelationshipType::USES };
	Relationship printRelation = Relationship{ printEntity, zEntity, RelationshipType::USES };


	vector<Relationship> expectedResult = vector<Relationship>{ procedureYRelation, assignRelation, procedureZRelation, printRelation };

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
	Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token zToken = Token{ "z", TokenType::NAME_OR_KEYWORD };
	Token printToken = Token{ PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token addToken = Token{ PLUS_OPERATOR, TokenType::OPERATOR };
	Token notEqualToken = Token{ NOT_EQUAL_OPERATOR, TokenType::OPERATOR };

	// Creating nodes
	shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
	shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(notEqualToken));
	shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
	
	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> addNode(new ExpressionASTNode(notEqualToken));
	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));
	shared_ptr<ASTNode> z(new VariableASTNode(zToken));
	
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
	Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());
	Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());

	Relationship whileXRelation = Relationship{ whileEntity, xEntity, RelationshipType::USES };
	Relationship whileYRelation = Relationship{ whileEntity, yEntity, RelationshipType::USES };
	Relationship whileZRelation = Relationship{ whileEntity, zEntity, RelationshipType::USES };
	Relationship assignRelation = Relationship{ assignEntity, yEntity, RelationshipType::USES };
	Relationship printRelation = Relationship{ printEntity, zEntity, RelationshipType::USES };


	vector<Relationship> expectedResult = vector<Relationship>{ whileXRelation, whileYRelation, whileZRelation, printRelation, whileYRelation, assignRelation };

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
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constToken = Token{ "0", TokenType::INTEGER };
	Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token equalityToken = Token{ EQUALITY_OPERATOR, TokenType::OPERATOR };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };

	// Creating nodes
	shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));

	shared_ptr<ASTNode> condNode(new ExpressionASTNode(equalityToken));
	shared_ptr<ASTNode> xCond(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

	shared_ptr<ASTNode> assignXNode(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> assignYNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTnode(stmtLst));
	shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> xThen(new VariableASTNode(xToken));
	shared_ptr<ASTNode> yThen(new VariableASTNode(yToken));

	shared_ptr<ASTNode> yElse (new VariableASTNode(yToken));
	shared_ptr<ASTNode> xElse(new VariableASTNode(xToken));

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
	Entity assignXEntity = Entity::createAssignEntity(assignXNode->getLineNumber());
	Entity assignYEntity = Entity::createAssignEntity(assignYNode->getLineNumber());

	Relationship ifCondXRelation = Relationship{ ifEntity, xCondEntity, RelationshipType::USES };
	Relationship ifThenYRelation = Relationship{ ifEntity, yThenEntity, RelationshipType::USES };
	Relationship ifElseXRelation = Relationship{ ifEntity, xElseEntity, RelationshipType::USES };
	Relationship assignXRelation = Relationship{ assignXEntity, yThenEntity, RelationshipType::USES };
	Relationship assignYRelation = Relationship{ assignYEntity, xElseEntity, RelationshipType::USES };


	vector<Relationship> expectedResult = vector<Relationship>{ ifCondXRelation, ifThenYRelation, assignXRelation, ifElseXRelation, assignYRelation };

	handleIf(ifNode, expectedResult);
}


TEST_CASE("UsesExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> rootNode, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.extract(rootNode);

		// Then
		//REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			cout << extractedResult[i].getLhs().toString() + " uses ";
			cout << extractedResult[i].getRhs().toString() + "\n";

			//REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

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
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
	Token notEqualToken = Token{ NOT_EQUAL_OPERATOR, TokenType::OPERATOR };
	Token greaterToken = Token{ GREATER_OPERATOR, TokenType::OPERATOR };
	Token minusToken = Token{ MINUS_OPERATOR, TokenType::OPERATOR };
	Token constThreeToken = Token{ "3", TokenType::INTEGER };	
	Token constZeroToken = Token{ "0", TokenType::INTEGER };
	Token constOneToken = Token{ "1", TokenType::INTEGER };
	Token constFiveToken = Token{ "5", TokenType::INTEGER };
	Token readToken = Token{ READ_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token printToken = Token{ PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token stmtListToken = Token{ "", TokenType::INVALID };
	
	// Creating AST nodes
	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

	// Line 1 (x = 3)
	shared_ptr<ASTNode> mainStmtList(new StatementListASTnode(stmtListToken));
	shared_ptr<ASTNode> x1Node(new VariableASTNode(xToken));
	shared_ptr<ASTNode> assign1Node(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> constThreeNode(new ConstantValueASTNode(constThreeToken));

	x1Node->setLineNumber(1);
	assign1Node->setLineNumber(1);
	constThreeNode->setLineNumber(1);

	// Line 2 (read y)
	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
	shared_ptr<ASTNode> y2Node(new VariableASTNode(yToken));

	readNode->setLineNumber(2);
	y2Node->setLineNumber(2);

	// Line 3 (while ( x != 0 ))
	shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
	shared_ptr<ASTNode> x3Node(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(constZeroToken));
	shared_ptr<ASTNode> notEqualNode(new ExpressionASTNode(notEqualToken));

	whileNode->setLineNumber(3);
	x3Node->setLineNumber(3);
	constZeroNode->setLineNumber(3);
	notEqualNode->setLineNumber(3);

	// Line 4 (x = x - 1)
	shared_ptr<ASTNode> whileStmtList(new StatementListASTnode(stmtListToken));
	shared_ptr<ASTNode> x4LhsNode(new VariableASTNode(xToken));
	shared_ptr<ASTNode> x4RhsNode(new VariableASTNode(xToken));
	shared_ptr<ASTNode> assign4Node(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> minusNode(new ExpressionASTNode(minusToken));
	shared_ptr<ASTNode> constOneNode(new ConstantValueASTNode(constOneToken));

	x4LhsNode->setLineNumber(4);
	x4RhsNode->setLineNumber(4);
	assign4Node->setLineNumber(4);
	minusNode->setLineNumber(4);
	constOneNode->setLineNumber(4);

	// Line 5 (print y)
	shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
	shared_ptr<ASTNode> y5Node(new VariableASTNode(yToken));

	printNode->setLineNumber(5);
	y5Node->setLineNumber(5);

	// Line 6 (if (y > 5) then)
	shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));
	shared_ptr<ASTNode> y6Node(new VariableASTNode(yToken));
	shared_ptr<ASTNode> greaterNode(new ExpressionASTNode(greaterToken));
	shared_ptr<ASTNode> constFiveNode(new ConstantValueASTNode(constFiveToken));

	ifNode->setLineNumber(6);
	y6Node->setLineNumber(6);
	greaterNode->setLineNumber(6);
	constFiveNode->setLineNumber(6);

	// Line 7 (x = y)
	shared_ptr<ASTNode> thenStmtList(new StatementListASTnode(stmtListToken));
	shared_ptr<ASTNode> assign7Node(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> x7Node(new VariableASTNode(xToken));
	shared_ptr<ASTNode> y7Node(new VariableASTNode(yToken));

	assign7Node->setLineNumber(7);
	x7Node->setLineNumber(7);
	y7Node->setLineNumber(7);

	// Line 8 (y = x)
	shared_ptr<ASTNode> elseStmtList(new StatementListASTnode(stmtListToken));
	shared_ptr<ASTNode> assign8Node(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> y8Node(new VariableASTNode(yToken));
	shared_ptr<ASTNode> x8Node(new VariableASTNode(xToken));
	
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

	Entity assign7Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
	Entity y7Entity = Entity::createVariableEntity(y7Node->getLineNumber(), yToken);

	Entity assign8Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
	Entity x8Entity = Entity::createVariableEntity(x8Node->getLineNumber(), xToken);
	
	// Creating relationships
	Relationship mainUsesXCond = Relationship{ procedureEntity, xCond, RelationshipType::USES };
	Relationship mainUsesX4 = Relationship{ procedureEntity, x4Entity, RelationshipType::USES };
	Relationship mainUsesY5 = Relationship{ procedureEntity, y5Entity, RelationshipType::USES };
	Relationship mainUsesYCond = Relationship{ procedureEntity, yCondEntity, RelationshipType::USES };
	Relationship mainUsesY7 = Relationship{ procedureEntity, y7Entity, RelationshipType::USES };
	Relationship mainUsesX8 = Relationship{ procedureEntity, x8Entity, RelationshipType::USES };

	Relationship whileUsesXCond = Relationship{ whileEntity, xCond, RelationshipType::USES };
	Relationship whileUsesX4 = Relationship{ whileEntity, x4Entity, RelationshipType::USES };
	Relationship whileUsesY5 = Relationship{ whileEntity, y5Entity, RelationshipType::USES };

	Relationship assign4UsesX4 = Relationship{ assign4Entity, x4Entity, RelationshipType::USES };
	Relationship printUsesY5 = Relationship{ printEntity, y5Entity, RelationshipType::USES };

	Relationship ifUsesYCond = Relationship{ ifEntity, yCondEntity, RelationshipType::USES };
	Relationship ifUsesY7 = Relationship{ ifEntity, y7Entity, RelationshipType::USES };
	Relationship ifUsesX8 = Relationship{ ifEntity, x8Entity, RelationshipType::USES };
	
	Relationship assign7UsesY7 = Relationship{ assign7Entity, y7Entity, RelationshipType::USES };
	Relationship assign8UsesX8 = Relationship{ assign8Entity, x8Entity, RelationshipType::USES };

	vector<Relationship> expectedResult = vector<Relationship>{ };


	testExtract(procedureNode, expectedResult);
}
