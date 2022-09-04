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
		Token expectedToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
		Entity expectedEntity = Entity{ EntityType::PROCEDURE, 1, expectedToken, expectedToken.getString() };

		vector<Token> tokens = vector<Token>{ Token{"procedure", TokenType::NAME_OR_KEYWORD} };
		shared_ptr<ASTNode> node(new ASTNode{ tokens });
		node->setType(ASTNodeType::PROCEDURE);
		node->setLineNumber(1);

		shared_ptr<ASTNode> child(new ASTNode(vector<Token>{expectedToken}));
		child->setLineNumber(1);
		node->addChild(child);

		testExtractEntity(node, expectedEntity);
	};

	SECTION("Test for 'read x") {
		Token expectedToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Entity expectedEntity = Entity{ EntityType::VARIABLE, 1, expectedToken, expectedToken.getString() };

		vector<Token> tokens = vector<Token>{ Token{"read", TokenType::NAME_OR_KEYWORD} };
		shared_ptr<ASTNode> node (new ASTNode(tokens));
		node->setType(ASTNodeType::READ);
		node->setLineNumber(1);

		shared_ptr <ASTNode> child = shared_ptr<ASTNode>(new ASTNode(vector<Token>{expectedToken}));
		child->setLineNumber(1);
		node->addChild(child);

		testExtractEntity(node, expectedEntity);
	}


	SECTION("Test for 'call addTwo'") {
		Token expectedToken = Token{ "addTwo", TokenType::NAME_OR_KEYWORD };
		Entity expectedEntity = Entity{ EntityType::PROCEDURE, 1, expectedToken, expectedToken.getString() };

		vector<Token> tokens = vector<Token>{ Token{"call", TokenType::NAME_OR_KEYWORD} };
		shared_ptr<ASTNode> node = shared_ptr<ASTNode>(new ASTNode({ tokens }));
		node->setType(ASTNodeType::CALL);
		node->setLineNumber(1);

		shared_ptr<ASTNode> child = shared_ptr<ASTNode>(new ASTNode(vector<Token>{expectedToken}));
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
	1.	read x;
	2.	y = 5;
	3.	print y;
	  }

	  procedure -> main & StmtLst
	  StmtLst -> read x -> x
	  StmtLst -> y = 5 -> = -> y & 5
	  StmtLst -> print y -> y

	*/

	// Create root node
	shared_ptr<ASTNode> root (new ASTNode( vector<Token> {Token{"procedure", TokenType::NAME_OR_KEYWORD}} ));
	root->setType(ASTNodeType::PROCEDURE);

	// Create name of procedure node
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	shared_ptr<ASTNode> main (new ASTNode(vector<Token> {mainToken}));
	main->setType(ASTNodeType::NAME);
	
	Entity mainEntity = Entity{ EntityType::PROCEDURE, -1, mainToken, mainToken.getString() };

	// Create stmtLst node
	shared_ptr<ASTNode> stmtLst(new ASTNode(vector<Token> {Token{ PROGRAM_KEYWORD, TokenType::DELIMITER }}));
	stmtLst->setType(ASTNodeType::STMTLIST);

	// Create read node
	shared_ptr<ASTNode> read (new ASTNode(vector<Token> {Token{READ_KEYWORD, TokenType::NAME_OR_KEYWORD}}));
	read->setType(ASTNodeType::READ);
	
	// Create x node
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	shared_ptr<ASTNode> x (new ASTNode(vector<Token> {xToken}));
	x->setType(ASTNodeType::NAME);

	read->setLineNumber(1);
	x->setLineNumber(1);

	Entity xEntity = Entity{ EntityType::VARIABLE, 1, xToken, xToken.getString() };

	// Create y1 node
	Token y1Token = Token{ "y", TokenType::NAME_OR_KEYWORD };
	shared_ptr<ASTNode> y1 (new ASTNode(vector<Token> {y1Token}));
	y1->setType(ASTNodeType::NAME);

	// Create assign node
	shared_ptr<ASTNode> assign (new ASTNode(vector<Token> {Token{"=", TokenType::OPERATOR}}));
	assign->setType(ASTNodeType::ASSIGN);

	// Create constant node
	Token constantToken = Token{ "5", TokenType::INTEGER };
	shared_ptr<ASTNode> constant (new ASTNode(vector<Token> {constantToken}));
	constant->setType(ASTNodeType::CONSTANT);

	y1->setLineNumber(2);
	assign->setLineNumber(2);
	constant->setLineNumber(2);

	Entity y1Entity = Entity{ EntityType::VARIABLE, 2, y1Token, y1Token.getString() };
	Entity constantEntity = Entity{ EntityType::CONSTANT, 2, constantToken, constantToken.getString() };

	shared_ptr<ASTNode> print (new ASTNode(vector<Token> {Token{ PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD }}));
	print->setType(ASTNodeType::PRINT);

	// Create y2 node
	Token y2Token = Token{ "y", TokenType::NAME_OR_KEYWORD };
	shared_ptr<ASTNode> y2 (new ASTNode(vector<Token> {y2Token}));
	y2->setType(ASTNodeType::NAME);

	print->setLineNumber(3);
	y2->setLineNumber(3);

	Entity y2Entity = Entity{ EntityType::VARIABLE, 3, y2Token, y2Token.getString() };


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
