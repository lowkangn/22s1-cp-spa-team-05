#include "catch.hpp"
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.cpp>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>

using namespace std;

TEST_CASE("ModifiesExtractor : test recursiveExtract") {
	auto testRecursiveExtract = [](Entity LHS, shared_ptr<ASTNode> RHS, vector<Relationship> expectedResult) {
		ModifiesExtractor extractor = ModifiesExtractor();

		vector<Relationship> extractedResult = extractor.recursiveExtract(LHS, RHS);

		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};

	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.asString()};

	// LHS x, RHS x + 1
	Token xToken = Token{ "x", TokenType::NAME };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };

	shared_ptr<ASTNode> addNode (new ASTNode( vector<Token> {addToken} ));
	addNode->setType(ASTNodeType::OPERATOR);

	shared_ptr<ASTNode> x (new ASTNode(vector<Token> {xToken}));
	x->setType(ASTNodeType::NAME);

	shared_ptr<ASTNode> constNode (new ASTNode(vector<Token> {constToken}));
	constNode->setType(ASTNodeType::CONSTANT);
	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken, constToken.asString() };


	addNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	addNode->addChild(x);
	addNode->addChild(constNode);

	Relationship xModifiesX = Relationship{ LHS, LHS, RelationshipType::MODIFIES };
	Relationship xModifiesConst = Relationship{ LHS, constEntity, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ xModifiesX, xModifiesConst };

	testRecursiveExtract(LHS, addNode, expectedResult);
}

TEST_CASE("ModifiesExtractor: test extractModifies") {


	auto testExtractModifies = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		ModifiesExtractor extractor = ModifiesExtractor();

		vector<Relationship> extractedResult = extractor.extractModifies(ast);

		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.asString() };

	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME };
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
	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken, constToken.asString() };


	addNode->setLineNumber(1);
	assignNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	assignNode->addChild(x);
	assignNode->addChild(addNode);

	addNode->addChild(x);
	addNode->addChild(constNode);


	Relationship xModifiesX = Relationship{ LHS, LHS, RelationshipType::MODIFIES };
	Relationship xModifiesConst = Relationship{ LHS, constEntity, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ xModifiesX, xModifiesConst };

	testExtractModifies(assignNode, expectedResult);

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

	Token leftToken = Token{ "x", TokenType::NAME };
	Entity LHS = Entity{ EntityType::VARIABLE, LINENUMBER, leftToken, leftToken.asString() };

	// x = x + 1 + y
	Token xToken = Token{ "x", TokenType::NAME };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token yToken = Token{ "y", TokenType::NAME };

	Entity yEntity = Entity{ EntityType::VARIABLE, LINENUMBER, yToken, yToken.asString() };

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
	Entity constEntity = Entity{ EntityType::CONSTANT, LINENUMBER, constToken, constToken.asString() };


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


	Relationship xModifiesX = Relationship{ LHS, LHS, RelationshipType::MODIFIES };
	Relationship xModifiesY = Relationship{ LHS, yEntity, RelationshipType::MODIFIES };
	Relationship xModifiesConst = Relationship{ LHS, constEntity, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ xModifiesX, xModifiesConst, xModifiesY };

	testExtract(assignNode, expectedResult);
}
