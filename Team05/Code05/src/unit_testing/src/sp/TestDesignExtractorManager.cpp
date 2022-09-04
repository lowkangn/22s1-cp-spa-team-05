#include "catch.hpp"
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/DesignExtractorManager.cpp>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/PatternExtractor.cpp>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>

TEST_CASE("DesignExtractor: test : extractEntity() && extractPattern()") {
	auto test = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntity, vector<Relationship> expectedRelationship) {

		shared_ptr<EntityExtractor> entityExtractor (new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor (new PatternExtractor());

		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());

		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor };

		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, relationExtractors);
		extractor.setRootNode(nodeToExtractFrom);

		vector<Entity> entities = extractor.extractEntities(nodeToExtractFrom);
		vector<Relationship> relationships = extractor.extractRelationships(nodeToExtractFrom);

		REQUIRE(entities.size() == expectedEntity.size());
		REQUIRE(relationships.size() == expectedRelationship.size());

		for (int i = 0; i < entities.size(); i++) {
			REQUIRE(entities[i].equals(expectedEntity[i]));
		}

		for (int i = 0; i < relationships.size(); i++) {
			REQUIRE(relationships[i].equals(expectedRelationship[i]));
		}
	};

	/*
		procedure main {
		1.	read y;
		2.	x = 1;
		}
	*/

	Token programToken = Token{ PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token procedureToken = Token{ PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token procedureNameToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token stmtLstToken = Token("", TokenType::DELIMITER);
	Token xToken = Token("x", TokenType::NAME_OR_KEYWORD);
	Token yToken = Token("y", TokenType::NAME_OR_KEYWORD);
	Token constant = Token("1", TokenType::INTEGER);
	Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
	Token assignToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);

	shared_ptr<ASTNode> nodeToExtractFrom(new ASTNode(vector<Token>{programToken}));
	nodeToExtractFrom->setType(ASTNodeType::PROGRAM);

	shared_ptr<ASTNode> procedureNode(new ASTNode(vector<Token>{procedureToken}));
	procedureNode->setType(ASTNodeType::PROCEDURE);

	shared_ptr<ASTNode> procedureNameNode(new ASTNode(vector<Token>{procedureNameToken}));
	procedureNameNode->setType(ASTNodeType::NAME);
	Entity procedureEntity = Entity{ EntityType::PROCEDURE, procedureNameNode->getLineNumber(), procedureNameToken, procedureNameToken.getString() };


	shared_ptr<ASTNode> stmtLstNode(new ASTNode(vector<Token>{stmtLstToken}));
	stmtLstNode->setType(ASTNodeType::STMTLIST);

	shared_ptr<ASTNode> xNode(new ASTNode(vector<Token>{xToken}));
	xNode->setType(ASTNodeType::NAME);
	xNode->setLineNumber(2);
	Entity xEntity = Entity{ EntityType::VARIABLE, xNode->getLineNumber(), xToken, xToken.getString() };

	shared_ptr<ASTNode> constantNode(new ASTNode(vector<Token>{constant}));
	constantNode->setType(ASTNodeType::CONSTANT);
	constantNode->setLineNumber(2);
	Entity constantEntity = Entity{ EntityType::CONSTANT, constantNode->getLineNumber(), constant, constant.getString() };


	shared_ptr<ASTNode> assignNode(new ASTNode(vector<Token>{assignToken}));
	assignNode->setType(ASTNodeType::ASSIGN);
	assignNode->setLineNumber(2);

	shared_ptr<ASTNode> yNode(new ASTNode(vector<Token>{yToken}));
	yNode->setType(ASTNodeType::NAME);
	yNode->setLineNumber(1);
	Entity yEntity = Entity{ EntityType::VARIABLE, yNode->getLineNumber(), yToken, yToken.getString() };


	shared_ptr<ASTNode> readNode(new ASTNode(vector<Token>{readToken}));
	readNode->setType(ASTNodeType::READ);
	readNode->setLineNumber(1);

	assignNode->addChild(xNode);
	assignNode->addChild(constantNode);

	readNode->addChild(yNode);

	stmtLstNode->addChild(readNode);
	stmtLstNode->addChild(assignNode);

	procedureNode->addChild(procedureNameNode);
	procedureNode->addChild(stmtLstNode);

	nodeToExtractFrom->addChild(procedureNode);

	Relationship procedureModifiesy = Relationship(procedureEntity, yEntity, RelationshipType::MODIFIES);
	Relationship procedureModifiesx = Relationship(procedureEntity, xEntity, RelationshipType::MODIFIES);


	vector<Entity> expectedEntities = vector<Entity>{ procedureEntity, yEntity, xEntity, constantEntity };
	vector<Relationship> expectedRelation = vector<Relationship>{ procedureModifiesy, procedureModifiesx };

	test(nodeToExtractFrom, expectedEntities, expectedRelation);
}
