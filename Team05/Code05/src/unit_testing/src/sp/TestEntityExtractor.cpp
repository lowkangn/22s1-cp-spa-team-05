#include "catch.hpp"
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/EntityExtractor.cpp>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>

using namespace std;

TEST_CASE("EntityExtractor: test extractEntity") {


	auto testExtractEntity = [](ASTNode nodeToExtractFrom, Entity expectedEntity) {
		EntityExtractor extractor = EntityExtractor();

		Entity extractedEntity = extractor.extractEntity(nodeToExtractFrom);

		REQUIRE(extractedEntity.equals(expectedEntity));
	};

	//TODO Create function for creating the expectedToken and expectedEntity

	// Test for "procedure main" works
	Token expectedToken = Token{ "main", TokenType::NAME };
	Entity expectedEntity = Entity{ EntityType::PROCEDURE, 1, expectedToken, expectedToken.asString() };
	
	vector<Token> tokens = vector<Token>{ Token{"procedure", TokenType::NAME} };
	ASTNode node = ASTNode{ tokens };
	node.setType(ASTNodeType::PROCEDURE);
	node.setLineNumber(1);

	ASTNode child = ASTNode{ vector<Token>{expectedToken} };
	child.setLineNumber(1);
	node.addChild(child);

	testExtractEntity(node, expectedEntity);

	// Test for "read x"
	expectedToken = Token{ "x", TokenType::NAME };
	expectedEntity = Entity{ EntityType::VARIABLE, 1, expectedToken, expectedToken.asString() };
	
	tokens = vector<Token>{ Token{"read", TokenType::NAME} };
	node = ASTNode{ tokens };
	node.setType(ASTNodeType::READ);
	node.setLineNumber(1);

	child = ASTNode{ vector<Token>{expectedToken} };
	child.setLineNumber(1);
	node.addChild(child);

	testExtractEntity(node, expectedEntity);

	// Test for "call addTwo"
	expectedToken = Token{ "addTwo", TokenType::NAME };
	expectedEntity = Entity{ EntityType::PROCEDURE, 1, expectedToken, expectedToken.asString() };

	tokens = vector<Token>{ Token{"call", TokenType::NAME} };
	node = ASTNode{ tokens };
	node.setType(ASTNodeType::CALL);
	node.setLineNumber(1);

	child = ASTNode{ vector<Token>{expectedToken} };
	child.setLineNumber(1);
	node.addChild(child);

	testExtractEntity(node, expectedEntity);

	// Test for "x = 5"
	expectedToken = Token{ "x", TokenType::NAME };
	expectedEntity = Entity{ EntityType::VARIABLE, 1, expectedToken, expectedToken.asString() };

	tokens = vector<Token>{ Token{"=", TokenType::OPERATOR} };
	node = ASTNode{ tokens };
	node.setType(ASTNodeType::ASSIGN);
	node.setLineNumber(1);

	child = ASTNode{ vector<Token>{expectedToken, Token{"5", TokenType::DELIMITER}} };
	child.setLineNumber(1);
	node.addChild(child);

	testExtractEntity(node, expectedEntity);

}
