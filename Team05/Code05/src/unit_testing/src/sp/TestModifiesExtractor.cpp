#include "catch.hpp"
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.cpp>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>
#include <vector>
#include <memory>

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
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.getString() };
	Token lineNumber = Token("1", TokenType::INTEGER);
	Entity lineEntity = Entity{ EntityType::LINENUMBER, LINENUMBER, lineNumber, lineNumber.getString() };

	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };

	shared_ptr<ASTNode> assignNode (new ASTNode(vector<Token> {assignToken}));
	assignNode->setType(ASTNodeType::ASSIGN);

	shared_ptr<ASTNode> addNode (new ASTNode(vector<Token> {addToken}));
	addNode->setType(ASTNodeType::OPERATOR);

	shared_ptr<ASTNode> x (new ASTNode(vector<Token> {xToken}));
	x->setType(ASTNodeType::NAME);

	shared_ptr<ASTNode> constNode (new ASTNode(vector<Token> {constToken}));
	constNode->setType(ASTNodeType::CONSTANT);
	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken, constToken.getString() };


	addNode->setLineNumber(1);
	assignNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	assignNode->addChild(x);
	assignNode->addChild(addNode);

	addNode->addChild(x);
	addNode->addChild(constNode);


	Relationship modifiesX = Relationship{ lineEntity, LHS, RelationshipType::MODIFIES };

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
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.getString() };

	// read x;
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };


	shared_ptr<ASTNode> readNode(new ASTNode(vector<Token> {readToken}));
	readNode->setType(ASTNodeType::READ);

	shared_ptr<ASTNode> x(new ASTNode(vector<Token> {xToken}));

	x->setType(ASTNodeType::NAME);

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	Token lineNumber = Token{ "1",TokenType::NAME_OR_KEYWORD };
	Entity lineEntity = Entity{ EntityType::LINENUMBER, 1, lineNumber, lineNumber.getString() };
	Entity xEntity = Entity{ EntityType::VARIABLE, 1, xToken, xToken.getString() };

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
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.getString() };

	/*
		procedure main {
			x = 1;
			read y;
		}
	*/
	Token procedureToken = { "procedure", TokenType::NAME_OR_KEYWORD };
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token stmtLst = Token{"", TokenType::INVALID};



	shared_ptr<ASTNode> readNode(new ASTNode(vector<Token> {readToken}));
	readNode->setType(ASTNodeType::READ);

	shared_ptr<ASTNode> procedureNode(new ASTNode(vector<Token> {procedureToken}));
	procedureNode->setType(ASTNodeType::PROCEDURE);

	shared_ptr<ASTNode> mainNode(new ASTNode(vector<Token> {mainToken}));
	mainNode->setType(ASTNodeType::PROCEDURE);

	shared_ptr<ASTNode> assignNode(new ASTNode(vector<Token> {assignToken}));
	assignNode->setType(ASTNodeType::ASSIGN);

	shared_ptr<ASTNode> stmtLstNode(new ASTNode(vector<Token> {stmtLst}));
	stmtLstNode->setType(ASTNodeType::STMTLIST);

	shared_ptr<ASTNode> x(new ASTNode(vector<Token> {xToken}));
	shared_ptr<ASTNode> y(new ASTNode(vector<Token> {yToken}));
	shared_ptr<ASTNode> constNode(new ASTNode(vector<Token> {constToken}));

	x->setType(ASTNodeType::NAME);
	y->setType(ASTNodeType::NAME);

	x->setLineNumber(1);
	constNode->setLineNumber(1);
	y->setLineNumber(2);
	readNode->setLineNumber(2);

	procedureNode->addChild(mainNode);
	procedureNode->addChild(stmtLstNode);

	stmtLstNode->addChild(assignNode);
	stmtLstNode->addChild(readNode);

	assignNode->addChild(x);
	assignNode->addChild(constNode);

	readNode->addChild(y);

	Entity procedureEntity = Entity{ EntityType::PROCEDURE, mainNode->getLineNumber(), mainToken, mainToken.getString() };
	Entity xEntity = Entity{ EntityType::VARIABLE, x->getLineNumber(), xToken, xToken.getString() };
	Entity yEntity = Entity{ EntityType::VARIABLE, y->getLineNumber(), yToken, yToken.getString() };

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
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.getString() };

	// x = x + 1 + y
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token lineNumber = Token("1", TokenType::INTEGER);
	Entity lineEntity = Entity{ EntityType::LINENUMBER, LINENUMBER, lineNumber, lineNumber.getString() };


	Entity yEntity = Entity{ EntityType::VARIABLE, LINENUMBER, yToken, yToken.getString() };

	shared_ptr<ASTNode> LHSNode (new ASTNode(vector<Token> {xToken}));

	shared_ptr<ASTNode> assignNode (new ASTNode(vector<Token> {assignToken}));
	assignNode->setType(ASTNodeType::ASSIGN);

	shared_ptr<ASTNode> addNode1 (new ASTNode(vector<Token> {addToken}));
	addNode1->setType(ASTNodeType::OPERATOR);	
	shared_ptr<ASTNode> addNode2 (new ASTNode(vector<Token> {addToken}));
	addNode2->setType(ASTNodeType::OPERATOR);

	shared_ptr<ASTNode> x (new ASTNode(vector<Token> {xToken}));
	shared_ptr<ASTNode> y (new ASTNode(vector<Token> {yToken}));
	x->setType(ASTNodeType::NAME);
	y->setType(ASTNodeType::NAME);

	shared_ptr<ASTNode> constNode (new ASTNode(vector<Token> {constToken}));
	constNode->setType(ASTNodeType::CONSTANT);
	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken, constToken.getString() };


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


	Relationship modifiesX = Relationship{ lineEntity, LHS, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

	testExtract(assignNode, expectedResult);
}
