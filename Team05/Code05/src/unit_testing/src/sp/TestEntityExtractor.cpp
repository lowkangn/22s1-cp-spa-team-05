#include "catch.hpp"
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/EntityExtractor.cpp>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>
#include <memory>

using namespace std;

TEST_CASE("EntityExtractor: test extractEntity") {


	auto testExtractEntity = [](shared_ptr<ASTNode> nodeToExtractFrom, Entity expectedEntity) {
		EntityExtractor extractor = EntityExtractor();

		Entity extractedEntity = extractor.extractEntity(nodeToExtractFrom);

		REQUIRE(extractedEntity.equals(expectedEntity));
	};

	//TODO Create function for creating the expectedToken and expectedEntity

	SECTION("Test for 'procedure main' works") {
		Token expectedToken = Token{ "main", TokenType::NAME };
		Entity expectedEntity = Entity{ EntityType::PROCEDURE, 1, expectedToken, expectedToken.asString() };

		vector<Token> tokens = vector<Token>{ Token{"procedure", TokenType::NAME} };
		shared_ptr<ASTNode> node(new ASTNode{ tokens });
		node->setType(ASTNodeType::PROCEDURE);
		node->setLineNumber(1);

		shared_ptr<ASTNode> child(new ASTNode(vector<Token>{expectedToken}));
		child->setLineNumber(1);
		node->addChild(child);

		testExtractEntity(node, expectedEntity);
	};

	SECTION("Test for 'read x") {
		Token expectedToken = Token{ "x", TokenType::NAME };
		Entity expectedEntity = Entity{ EntityType::VARIABLE, 1, expectedToken, expectedToken.asString() };

		vector<Token> tokens = vector<Token>{ Token{"read", TokenType::NAME} };
		shared_ptr<ASTNode> node (new ASTNode(tokens));
		node->setType(ASTNodeType::READ);
		node->setLineNumber(1);

		shared_ptr <ASTNode> child = shared_ptr<ASTNode>(new ASTNode(vector<Token>{expectedToken}));
		child->setLineNumber(1);
		node->addChild(child);

		testExtractEntity(node, expectedEntity);
	}


	SECTION("Test for 'call addTwo'") {
		Token expectedToken = Token{ "addTwo", TokenType::NAME };
		Entity expectedEntity = Entity{ EntityType::PROCEDURE, 1, expectedToken, expectedToken.asString() };

		vector<Token> tokens = vector<Token>{ Token{"call", TokenType::NAME} };
		shared_ptr<ASTNode> node = shared_ptr<ASTNode>(new ASTNode({ tokens }));
		node->setType(ASTNodeType::CALL);
		node->setLineNumber(1);

		shared_ptr<ASTNode> child = shared_ptr<ASTNode>(new ASTNode(vector<Token>{expectedToken}));
		child->setLineNumber(1);
		node->addChild(child);

		testExtractEntity(node, expectedEntity);
	}


	SECTION("Test for 'x = 5'") {
		Token expectedToken = Token{ "x", TokenType::NAME };
		Entity expectedEntity = Entity{ EntityType::VARIABLE, 1, expectedToken, expectedToken.asString() };

		vector<Token> tokens = vector<Token>{ Token{"=", TokenType::OPERATOR} };
		shared_ptr<ASTNode> node = shared_ptr<ASTNode>(new ASTNode({ tokens }));
		node->setType(ASTNodeType::ASSIGN);
		node->setLineNumber(1);

		shared_ptr<ASTNode> child = shared_ptr<ASTNode>(new ASTNode(vector<Token>{expectedToken, Token{ "5", TokenType::DELIMITER }}));
		child->setLineNumber(1);
		node->addChild(child);

		testExtractEntity(node, expectedEntity);
	}


}

TEST_CASE("EntityExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntityVector) {
		EntityExtractor extractor = EntityExtractor();

		vector<Entity> extractedEntity = extractor.extract(nodeToExtractFrom);

		REQUIRE(extractedEntity.size() == expectedEntityVector.size());

		for (int i = 0; i < extractedEntity.size(); i++) {
			REQUIRE(extractedEntity[i].equals(expectedEntityVector[i]));
		}
	};

	// TODO Create a function to generate the AST

	/*
	* The following is the SIMPLE Program
	* procedure main {
		read x
		read y
		z = 5;
	  }

	  procedure -> main & StmtLst
	  StmtLst -> read x -> x
	  StmtLst -> y = 5 -> = -> y & 5
	  StmtLst -> print y -> y

	*/

	shared_ptr<ASTNode> root (new ASTNode( vector<Token> {Token{"procedure", TokenType::NAME}} ));
	root->setType(ASTNodeType::PROCEDURE);
	Token mainToken = Token{ "main", TokenType::NAME };
	shared_ptr<ASTNode> main (new ASTNode(vector<Token> {mainToken}));
	main->setType(ASTNodeType::NAME);
	root->setLineNumber(1);
	main->setLineNumber(1);

	Entity mainEntity = Entity{ EntityType::PROCEDURE, 1, mainToken, mainToken.asString() };

	shared_ptr<ASTNode> stmtLst(new ASTNode(vector<Token> {Token{ "", TokenType::INVALID }}));
	stmtLst->setType(ASTNodeType::STMTLIST);

	shared_ptr<ASTNode> read (new ASTNode(vector<Token> {Token{"read", TokenType::NAME}}));
	read->setType(ASTNodeType::READ);
	Token xToken = Token{ "x", TokenType::NAME };
	shared_ptr<ASTNode> x (new ASTNode(vector<Token> {xToken}));
	x->setType(ASTNodeType::NAME);
	read->setLineNumber(2);
	x->setLineNumber(2);

	Entity xEntity = Entity{ EntityType::VARIABLE, 2, xToken, xToken.asString() };

	Token y1Token = Token{ "y", TokenType::NAME };
	shared_ptr<ASTNode> y1 (new ASTNode(vector<Token> {y1Token}));
	y1->setType(ASTNodeType::NAME);
	shared_ptr<ASTNode> assign (new ASTNode(vector<Token> {Token{"=", TokenType::OPERATOR}}));
	assign->setType(ASTNodeType::ASSIGN);
	Token constantToken = Token{ "5", TokenType::INTEGER };
	shared_ptr<ASTNode> constant (new ASTNode(vector<Token> {constantToken}));
	constant->setType(ASTNodeType::CONSTANT);
	y1->setLineNumber(3);
	assign->setLineNumber(3);
	constant->setLineNumber(3);

	Entity y1Entity = Entity{ EntityType::VARIABLE, 3, y1Token, y1Token.asString() };
	Entity constantEntity = Entity{ EntityType::CONSTANT, 3, constantToken, constantToken.asString() };

	shared_ptr<ASTNode> print (new ASTNode(vector<Token> {Token{ "print", TokenType::NAME }}));
	print->setType(ASTNodeType::PRINT);
	Token y2Token = Token{ "y", TokenType::NAME };
	shared_ptr<ASTNode> y2 (new ASTNode(vector<Token> {y2Token}));
	y2->setType(ASTNodeType::NAME);
	print->setLineNumber(4);
	y2->setLineNumber(4);

	Entity y2Entity = Entity{ EntityType::VARIABLE, 4, y2Token, y2Token.asString() };


	root->addChild(main);
	root->addChild(stmtLst);

	stmtLst->addChild(read);
	stmtLst->addChild(assign);
	stmtLst->addChild(print);

	read->addChild(x);

	assign->addChild(y1);
	assign->addChild(constant);

	print->addChild(y2);

	vector<Entity> expectedVector = vector<Entity>{mainEntity, xEntity, y1Entity, constantEntity, y2Entity};

	testExtract(root, expectedVector);
}
