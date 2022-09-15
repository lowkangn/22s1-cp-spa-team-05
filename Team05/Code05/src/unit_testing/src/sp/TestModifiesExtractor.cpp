#include "catch.hpp"
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>

using namespace std;

TEST_CASE("ModifiesExtractor: test handleAssign") {


	auto handleAssign = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleAssign(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };

	Entity assignEntity = Entity::createAssignEntity(1);

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> addNode(new ExpressionASTNode(addToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	Entity constEntity = Entity::createConstantEntity(LINENUMBER, constToken);


	addNode->setLineNumber(1);
	assignNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	assignNode->addChild(x);
	assignNode->addChild(addNode);

	addNode->addChild(x);
	addNode->addChild(constNode);


	Relationship modifiesX = Relationship{ assignEntity, LHS, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

	handleAssign(assignNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleRead") {


	auto testHandleRead = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleRead(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// read x;
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	Entity lineEntity = Entity::createReadEntity(1);;
	Entity xEntity = Entity::createVariableEntity(1, xToken);

	Relationship readRelation = Relationship{ lineEntity, xEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ readRelation };

	testHandleRead(readNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleProcedure") {


	auto handleProcedure = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleProcedure(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			x = 1;
			read y;
		}
	*/
	// Creating tokens
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };


	// Creating nodes
	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));
	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

	x->setLineNumber(1);
	constNode->setLineNumber(1);
	y->setLineNumber(2);
	readNode->setLineNumber(2);

	procedureNode->addChild(stmtLstNode);

	stmtLstNode->addChild(assignNode);
	stmtLstNode->addChild(readNode);

	assignNode->addChild(x);
	assignNode->addChild(constNode);

	readNode->addChild(y);

	// Creating Relationship
	Entity procedureEntity = Entity::createProcedureEntity(mainToken);
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());
	Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

	Relationship procedureXRelation = Relationship{ procedureEntity, xEntity, RelationshipType::MODIFIES };
	Relationship procedureYRelation = Relationship{ procedureEntity, yEntity, RelationshipType::MODIFIES };
	Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
	Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ procedureXRelation, procedureYRelation };

	handleProcedure(procedureNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleWhile") {

	auto test = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleWhile(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			1. while ( x != 0 ) {
			2. 	x = x - 1
			}
		}
	*/

	// Creating tokens
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token zeroToken = Token{ "1", TokenType::INTEGER };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
	Token minusToken = Token{ "-", TokenType::OPERATOR};
	Token equalityToken = Token{ "!=", TokenType::OPERATOR};
	Token stmtLst = Token::getPlaceHolderToken();


	// Creating nodes
	shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
	whileNode->setLineNumber(1);

	shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(equalityToken));
	conditionNode->setLineNumber(1);

	shared_ptr<ASTNode> xCond(new VariableASTNode(xToken));
	xCond->setLineNumber(1);

	
	shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(zeroToken));
	constZeroNode->setLineNumber(1);


	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
	assignNode->setLineNumber(2);


	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	x->setLineNumber(2);


	shared_ptr<ASTNode> exprNode(new ExpressionASTNode(minusToken));
	conditionNode->setLineNumber(2);


	shared_ptr<ASTNode> xMinus(new VariableASTNode(xToken));
	xMinus->setLineNumber(2);


	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	constNode->setLineNumber(2);


	whileNode->addChild(conditionNode);
	whileNode->addChild(whileStmtLstNode);
	whileStmtLstNode->addChild(assignNode);

	conditionNode->addChild(xCond);
	conditionNode->addChild(constZeroNode);

	assignNode->addChild(x);
	assignNode->addChild(exprNode);

	exprNode->addChild(xMinus);
	exprNode->addChild(constNode);

	// Creating Relationships
	Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());

	Relationship whileRelation = Relationship{ whileEntity, xEntity, RelationshipType::MODIFIES };
	Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ whileRelation };

	test(whileNode, expectedResult);
}

TEST_CASE("ModifiesExtractor: test handleIf") {


	auto handleProcedure = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleIf(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			1. if (x == 0) then {
			2. 	   x = 1;
			}  else {
			3.     y = 2;
			}
		}
	*/
	// Creating tokens
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constZeroToken = Token{ "0", TokenType::INTEGER };
	Token constOneToken = Token{ "1", TokenType::INTEGER };
	Token constTwoToken = Token{ "2", TokenType::INTEGER };
	Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token equalityToken = Token{ "==", TokenType::OPERATOR};
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };

	// Creating nodes
	shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));

	shared_ptr<ASTNode> condNode(new ExpressionASTNode(equalityToken));
	shared_ptr<ASTNode> xCond(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(constZeroToken));

	shared_ptr<ASTNode> assignXNode(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> assignYNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTnode(stmtLst));
	shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constOneNode(new ConstantValueASTNode(constOneToken));
	
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));
	shared_ptr<ASTNode> constTwoNode(new ConstantValueASTNode(constTwoToken));

	ifNode->addChild(condNode);
	ifNode->addChild(thenStmtLstNode);
	ifNode->addChild(elseStmtLstNode);

	condNode->addChild(xCond);
	condNode->addChild(constZeroNode);

	thenStmtLstNode->addChild(assignXNode);
	assignXNode->addChild(x);
	assignXNode->addChild(constOneNode);

	elseStmtLstNode->addChild(assignYNode);
	assignYNode->addChild(y);
	assignYNode->addChild(constTwoNode);

	// Creating Relationships
	Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity assignXEntity = Entity::createAssignEntity(assignXNode->getLineNumber());
	Entity assignYEntity = Entity::createAssignEntity(assignYNode->getLineNumber());

	Relationship xRelation = Relationship{ ifEntity, xEntity, RelationshipType::MODIFIES };
	Relationship yRelation = Relationship{ ifEntity, yEntity, RelationshipType::MODIFIES };
	Relationship assignXRelation = Relationship{ assignXEntity, xEntity, RelationshipType::MODIFIES };
	Relationship assignYRelation = Relationship{ assignYEntity, yEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ xRelation, yRelation };

	handleProcedure(ifNode, expectedResult);

}


TEST_CASE("ModifiesExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> rootNode, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.extract(rootNode);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			bool result = extractedResult[i].equals(expectedResult[i]);
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

	SECTION("Test 1") {
		const int LINENUMBER = 1;

		Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

		// x = x + 1 + y
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token addToken = Token{ "+", TokenType::OPERATOR };
		Token constToken = Token{ "1", TokenType::INTEGER };
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
		Entity assignEntity = Entity::createAssignEntity(LINENUMBER);;


		Entity yEntity = Entity::createVariableEntity(LINENUMBER, yToken);

		shared_ptr<ASTNode> LHSNode(new VariableASTNode(xToken));

		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

		shared_ptr<ASTNode> addNode1(new ExpressionASTNode(addToken));
		shared_ptr<ASTNode> addNode2(new ExpressionASTNode(addToken));

		shared_ptr<ASTNode> x(new VariableASTNode(xToken));
		shared_ptr<ASTNode> y(new VariableASTNode(yToken));

		shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

		Entity constEntity = Entity::createConstantEntity(LINENUMBER, constToken);


		addNode1->setLineNumber(1);
		assignNode->setLineNumber(1);
		x->setLineNumber(1);
		y->setLineNumber(1);
		constNode->setLineNumber(1);

		assignNode->addChild(x);
		assignNode->addChild(addNode1);

		addNode1->addChild(x);
		addNode1->addChild(addNode2);
		addNode2->addChild(constNode);
		addNode2->addChild(y);


		Relationship modifiesX = Relationship{ assignEntity, LHS, RelationshipType::MODIFIES };

		vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

		testExtract(assignNode, expectedResult);
	}

	SECTION("KN Test") {
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

		Entity assign1Entity = Entity::createAssignEntity(assign1Node->getLineNumber());
		Entity x1Entity = Entity::createVariableEntity(x1Node->getLineNumber(), xToken);
		Entity y2Entity = Entity::createVariableEntity(y2Node->getLineNumber(), yToken);
		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

		Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());

		Entity assign4Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
		Entity x4Entity = Entity::createVariableEntity(x4RhsNode->getLineNumber(), xToken);
		Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());
		Entity y5Entity = Entity::createVariableEntity(y5Node->getLineNumber(), yToken);

		Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());

		Entity assign7Entity = Entity::createAssignEntity(assign7Node->getLineNumber());
		Entity x7Entity = Entity::createVariableEntity(x7Node->getLineNumber(), xToken);

		Entity assign8Entity = Entity::createAssignEntity(assign8Node->getLineNumber());
		Entity y8Entity = Entity::createVariableEntity(y8Node->getLineNumber(), yToken);

		// Creating relationships
		Relationship mainModifiesX1 = Relationship{ procedureEntity, x1Entity, RelationshipType::MODIFIES };
		Relationship mainModifiesY2 = Relationship{ procedureEntity, y2Entity, RelationshipType::MODIFIES };
		Relationship mainModifiesX4 = Relationship{ procedureEntity, x4Entity, RelationshipType::MODIFIES };
		Relationship mainModifiesX7 = Relationship{ procedureEntity, x7Entity, RelationshipType::MODIFIES };
		Relationship mainModifiesY8 = Relationship{ procedureEntity, y8Entity, RelationshipType::MODIFIES };

		Relationship readModifiesX4 = Relationship{ readEntity, y2Entity, RelationshipType::MODIFIES };

		Relationship whileModifiesX4 = Relationship{ whileEntity, x4Entity, RelationshipType::MODIFIES};

		Relationship assign1ModifiesX1 = Relationship{ assign1Entity, x1Entity, RelationshipType::MODIFIES };

		Relationship assign4ModifiesX4 = Relationship{ assign4Entity, x4Entity, RelationshipType::MODIFIES };

		Relationship ifModifiesX7 = Relationship{ ifEntity, x7Entity, RelationshipType::MODIFIES };
		Relationship ifModifiesY8 = Relationship{ ifEntity, y8Entity, RelationshipType::MODIFIES };

		Relationship assign7ModifiesX7 = Relationship{ assign7Entity, x7Entity, RelationshipType::MODIFIES };
		Relationship assign8ModifiesY8 = Relationship{ assign8Entity, y8Entity, RelationshipType::MODIFIES };

		vector<Relationship> expectedResult = vector<Relationship>{ mainModifiesX1, mainModifiesY2, mainModifiesX4, mainModifiesX7, 
																	mainModifiesY8, assign1ModifiesX1, readModifiesX4, whileModifiesX4, assign4ModifiesX4,
																	ifModifiesX7, ifModifiesY8, assign7ModifiesX7, assign8ModifiesY8 };


		testExtract(procedureNode, expectedResult);
	}
}