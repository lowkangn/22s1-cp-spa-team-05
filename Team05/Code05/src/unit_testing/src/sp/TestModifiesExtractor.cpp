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
#include <sp/dataclasses/ast/ProgramASTNode.h>

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

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// x = x + 1
	Token xToken = Token::createNameOrKeywordToken("x");
	Token addToken = Token::createPlusToken();
	Token constToken = Token::createIntegerToken("1");
	Token assignToken = Token::createEqualsToken();

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


	Relationship modifiesX = Relationship::createModifiesRelationship(assignEntity, LHS);

	vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

	handleAssign(assignNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleAssign exceptions") {

	auto handleAssignException = [](shared_ptr<ASTNode> ast) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();
		
		REQUIRE_THROWS_AS(extractor.handleAssign(ast), ASTException);
	};

	// Test exception on valid read node for handleAssign
	const int LINENUMBER = 1;

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// read x;
	Token xToken = Token::createNameOrKeywordToken("x");
	Token readToken = Token::createReadToken();


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	Entity lineEntity = Entity::createReadEntity(1);;
	Entity xEntity = Entity::createVariableEntity(1, xToken);

	Relationship readRelation = Relationship::createModifiesRelationship(lineEntity, xEntity);


	vector<Relationship> expectedResult = vector<Relationship>{ readRelation };


	handleAssignException(readNode);
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

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// read x;
	Token xToken = Token::createNameOrKeywordToken("x");
	Token readToken = Token::createReadToken();


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	Entity lineEntity = Entity::createReadEntity(1);;
	Entity xEntity = Entity::createVariableEntity(1, xToken);

	Relationship readRelation = Relationship::createModifiesRelationship(lineEntity, xEntity);

	vector<Relationship> expectedResult = vector<Relationship>{ readRelation };

	testHandleRead(readNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test read exceptions") {
	auto handleReadException = [](shared_ptr<ASTNode> ast) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		REQUIRE_THROWS_AS(extractor.handleRead(ast), ASTException);
	};

	const int LINENUMBER = 1;

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// should throw exception on non-read node e.g. assign node
	// x = x + 1

	Token xToken = Token::createNameOrKeywordToken("x");
	Token addToken = Token::createPlusToken();
	Token constToken = Token::createIntegerToken("1");
	Token assignToken = Token::createEqualsToken();

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


	Relationship modifiesX = Relationship::createModifiesRelationship(assignEntity, LHS);

	vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

	handleReadException(assignNode);
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

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			x = 1;
			read y;
		}
	*/
	// Creating tokens

	Token mainToken = Token::createNameOrKeywordToken("main");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constToken = Token::createIntegerToken("1");
	Token readToken = Token::createReadToken();
	Token assignToken = Token::createEqualsToken();
	Token stmtLst = Token::createPlaceholderToken();

	// Creating nodes
	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTNode(stmtLst));

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

	Relationship procedureXRelation = Relationship::createModifiesRelationship(procedureEntity, xEntity);
	Relationship procedureYRelation = Relationship::createModifiesRelationship(procedureEntity, yEntity);
	Relationship assignRelation = Relationship::createModifiesRelationship(assignEntity, xEntity);
	Relationship readRelation = Relationship::createModifiesRelationship(readEntity, yEntity);


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

	Token leftToken = Token::createNameOrKeywordToken("x");
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			1. while ( x != 0 ) {
			2. 	x = x - 1
			}
		}
	*/

	// Creating tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token zeroToken = Token::createIntegerToken("0");
	Token constToken = Token::createIntegerToken("1");
	Token whileToken = Token::createWhileToken();
	Token assignToken = Token::createEqualsToken();
	Token minusToken = Token::createMinusToken();
	Token equalityToken = Token::createNotEqualsToken();
	Token stmtLst = Token::getPlaceHolderToken();


	// Creating nodes
	shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
	whileNode->setLineNumber(1);

	shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTNode(stmtLst));

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

	Relationship whileRelation = Relationship::createModifiesRelationship(whileEntity, xEntity);
	Relationship assignRelation = Relationship::createModifiesRelationship(assignEntity, xEntity);


	vector<Relationship> expectedResult = vector<Relationship>{ whileRelation };

	test(whileNode, expectedResult);
}

TEST_CASE("ModifiesExtractor: test handleIf") {


	auto handleIf = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
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

	Token leftToken = Token::createNameOrKeywordToken("x");
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
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constZeroToken = Token::createIntegerToken("0");
	Token constOneToken = Token::createIntegerToken("1");
	Token constTwoToken = Token::createIntegerToken("2");
	Token ifToken = Token::createIfToken();
	Token equalityToken = Token::createEqualityToken();
	Token assignToken = Token::createEqualsToken();
	Token stmtLst = Token::createPlaceholderToken();

	// Creating nodes
	shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));

	shared_ptr<ASTNode> condNode(new ExpressionASTNode(equalityToken));
	shared_ptr<ASTNode> xCond(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(constZeroToken));

	shared_ptr<ASTNode> assignXNode(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> assignYNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTNode(stmtLst));
	shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTNode(stmtLst));

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

	Relationship xRelation = Relationship::createModifiesRelationship(ifEntity, xEntity);
	Relationship yRelation = Relationship::createModifiesRelationship(ifEntity, yEntity);
	Relationship assignXRelation = Relationship::createModifiesRelationship(assignXEntity, xEntity);
	Relationship assignYRelation = Relationship::createModifiesRelationship(assignYEntity, yEntity);


	vector<Relationship> expectedResult = vector<Relationship>{ xRelation, yRelation };

	handleIf(ifNode, expectedResult);

}


TEST_CASE("ModifiesExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> rootNode, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.extract(rootNode);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());
		
		/* 
			Refactored this test case: cannot guarantee that order of traversal in extractedResult same as order in expectedResult. 
			Generalised this test case to check if extracted relationships are contained in expected result
		*/
		for (int i = 0; i < extractedResult.size(); i++) {
			bool isInExpectedResult = false;
			for (int j = 0; j < expectedResult.size(); j++) {
				isInExpectedResult |= extractedResult[i].equals(expectedResult[j]);
			}
			REQUIRE(isInExpectedResult);
		}
		
	};

	const int LINENUMBER = 1;

		Token leftToken = Token::createNameOrKeywordToken("x");
		Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

		// x = x + 1 + y
		Token xToken = Token::createNameOrKeywordToken("x");
		Token addToken = Token::createPlusToken();
		Token constToken = Token::createIntegerToken("1");
		Token assignToken = Token::createEqualsToken();
		Token yToken = Token::createNameOrKeywordToken("y");
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


		Relationship modifiesX = Relationship::createModifiesRelationship(assignEntity, LHS);

		vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

		testExtract(assignNode, expectedResult);

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
		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

		// Line 1 (x = 3)
		shared_ptr<ASTNode> mainStmtList(new StatementListASTNode(stmtListToken));
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
		shared_ptr<ASTNode> whileStmtList(new StatementListASTNode(stmtListToken));
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
		shared_ptr<ASTNode> thenStmtList(new StatementListASTNode(stmtListToken));
		shared_ptr<ASTNode> assign7Node(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> x7Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> y7Node(new VariableASTNode(yToken));

		assign7Node->setLineNumber(7);
		x7Node->setLineNumber(7);
		y7Node->setLineNumber(7);

		// Line 8 (y = x)
		shared_ptr<ASTNode> elseStmtList(new StatementListASTNode(stmtListToken));
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
		Relationship mainModifiesX1 = Relationship::createModifiesRelationship(procedureEntity, x1Entity);
		Relationship mainModifiesY2 = Relationship::createModifiesRelationship(procedureEntity, y2Entity);
		Relationship mainModifiesX4 = Relationship::createModifiesRelationship(procedureEntity, x4Entity);
		Relationship mainModifiesX7 = Relationship::createModifiesRelationship(procedureEntity, x7Entity);
		Relationship mainModifiesY8 = Relationship::createModifiesRelationship(procedureEntity, y8Entity);

		Relationship readModifiesX4 = Relationship::createModifiesRelationship(readEntity, y2Entity);

		Relationship whileModifiesX4 = Relationship::createModifiesRelationship(whileEntity, x4Entity);

		Relationship assign1ModifiesX1 = Relationship::createModifiesRelationship(assign1Entity, x1Entity);

		Relationship assign4ModifiesX4 = Relationship::createModifiesRelationship(assign4Entity, x4Entity);

		Relationship ifModifiesX7 = Relationship::createModifiesRelationship(ifEntity, x7Entity);
		Relationship ifModifiesY8 = Relationship::createModifiesRelationship(ifEntity, y8Entity);

		Relationship assign7ModifiesX7 = Relationship::createModifiesRelationship(assign7Entity, x7Entity);
		Relationship assign8ModifiesY8 = Relationship::createModifiesRelationship(assign8Entity, y8Entity);

		vector<Relationship> expectedResult = vector<Relationship>{ mainModifiesX1, mainModifiesY2, mainModifiesX4, mainModifiesX7,
																	mainModifiesY8, assign1ModifiesX1, readModifiesX4, whileModifiesX4, assign4ModifiesX4,
																	ifModifiesX7, ifModifiesY8, assign7ModifiesX7, assign8ModifiesY8 };


		testExtract(procedureNode, expectedResult);
	}
}