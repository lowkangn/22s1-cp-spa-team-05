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
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <iostream>

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
	Token addToken = Token{ "+", TokenType::OPERATOR };
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
