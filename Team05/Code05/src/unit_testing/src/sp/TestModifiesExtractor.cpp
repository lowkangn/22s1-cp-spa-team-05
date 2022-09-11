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
#include <sp/dataclasses/ast/ExpressionASTNode.cpp>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>

using namespace std;

TEST_CASE("ModifiesExtractor: test handleAssign") {


	auto handleAssign = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		ModifiesExtractor extractor = ModifiesExtractor();

		vector<Relationship> extractedResult = extractor.handleAssign(ast);

		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken };

	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };

	Entity assignEntity = Entity(EntityType::ASSIGN, 1, assignToken);

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> addNode(new ExpressionASTNode(addToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken };


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
		ModifiesExtractor extractor = ModifiesExtractor();

		vector<Relationship> extractedResult = extractor.handleRead(ast);

		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken };

	// read x;
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	Entity lineEntity = Entity{ EntityType::READ, 1, readToken };
	Entity xEntity = Entity{ EntityType::VARIABLE, 1, xToken };

	Relationship readRelation = Relationship{ lineEntity, xEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ readRelation };

	testHandleRead(readNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleProcedure") {


	auto handleProcedure = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		ModifiesExtractor extractor = ModifiesExtractor();

		vector<Relationship> extractedResult = extractor.handleProcedure(ast);

		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken };

	/*
		procedure main {
			x = 1;
			read y;
		}
	*/
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };



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

	Entity procedureEntity = Entity{ EntityType::PROCEDURE, procedureNode->getLineNumber(), mainToken };
	Entity xEntity = Entity{ EntityType::VARIABLE, x->getLineNumber(), xToken };
	Entity yEntity = Entity{ EntityType::VARIABLE, y->getLineNumber(), yToken };

	Relationship readRelation = Relationship{ procedureEntity, xEntity, RelationshipType::MODIFIES };
	Relationship assignRelation = Relationship{ procedureEntity, yEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ readRelation, assignRelation };

	handleProcedure(procedureNode, expectedResult);

}


TEST_CASE("ModifiesExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> rootNode, vector<Relationship> expectedResult) {
		ModifiesExtractor extractor = ModifiesExtractor();

		vector<Relationship> extractedResult = extractor.extract(rootNode);

		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}
	};

	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken };

	// x = x + 1 + y
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Entity assignEntity = Entity{ EntityType::ASSIGN, LINENUMBER, assignToken };


	Entity yEntity = Entity{ EntityType::VARIABLE, LINENUMBER, yToken };

	shared_ptr<ASTNode> LHSNode(new VariableASTNode(xToken));

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> addNode1(new ExpressionASTNode(addToken));
	shared_ptr<ASTNode> addNode2(new ExpressionASTNode(addToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));

	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken };


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