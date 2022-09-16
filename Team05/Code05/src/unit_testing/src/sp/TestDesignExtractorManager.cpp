#include "catch.hpp"
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <pkb/interfaces/PKBUpdateHandler.h>

TEST_CASE("DesignExtractor: test : extractEntity()") {
	auto test = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntity) {
		//Given
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor };

		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, relationExtractors);

		extractor.setRootNode(nodeToExtractFrom);

		// When
		vector<Entity> entities = extractor.extractEntities(nodeToExtractFrom);

		// Then
		REQUIRE(entities.size() == expectedEntity.size());

		for (int i = 0; i < entities.size(); i++) {
			REQUIRE(entities[i].equals(expectedEntity[i]));
		}
	};

	/*
		procedure main {
		1.	read y;
		2.	x = 1;
		}
	*/
	// Creating tokens
	Token programToken = Token::createProgramToken();
	Token procedureToken = Token::createProcedureToken();
	Token procedureNameToken = Token::createNameOrKeywordToken("main");
	Token stmtLstToken = Token::createPlaceholderToken();
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constant = Token::createIntegerToken("1");
	Token readToken = Token::createReadToken();
	Token assignToken = Token::createEqualsToken();

	// Creating nodes
	shared_ptr<ASTNode> nodeToExtractFrom(new ProgramASTNode(programToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(procedureNameToken));
	Entity procedureEntity = Entity::createProcedureEntity(procedureNameToken);

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTNode(stmtLstToken));

	shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
	xNode->setLineNumber(2);
	Entity xEntity = Entity::createVariableEntity(2, xToken);

	shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constant));
	constantNode->setLineNumber(2);
	Entity constantEntity = Entity::createConstantEntity(2, constant);


	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
	assignNode->setLineNumber(2);
	Entity assignEntity = Entity::createAssignEntity(2);

	shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
	yNode->setLineNumber(1);
	Entity yEntity = Entity::createVariableEntity(1, yToken);


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
	readNode->setLineNumber(1);
	Entity readEntity = Entity::createReadEntity(1);


	assignNode->addChild(xNode);
	assignNode->addChild(constantNode);

	readNode->addChild(yNode);

	stmtLstNode->addChild(readNode);
	stmtLstNode->addChild(assignNode);

	procedureNode->addChild(stmtLstNode);

	nodeToExtractFrom->addChild(procedureNode);

	// Creating relationships
	vector<Entity> expectedEntities = vector<Entity>{ procedureEntity, readEntity, yEntity, assignEntity, xEntity, constantEntity };

	test(nodeToExtractFrom, expectedEntities);
}

TEST_CASE("DesignExtractor: test : extractRelationships()") {
	auto test = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Relationship> expectedRelationship) {
		// Given
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor };

		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, relationExtractors);
		extractor.setRootNode(nodeToExtractFrom);

		// When
		vector<Relationship> relationships = extractor.extractRelationships(nodeToExtractFrom);

		// Then
		REQUIRE(relationships.size() == expectedRelationship.size());

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

	// Creating tokens
	Token programToken = Token::createProgramToken();;
	Token procedureToken = Token::createProcedureToken();;
	Token procedureNameToken = Token::createNameOrKeywordToken("main");
	Token stmtLstToken = Token::createPlaceholderToken();
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constant = Token::createIntegerToken("1");
	Token readToken = Token::createReadToken();
	Token assignToken = Token::createEqualsToken();

	// Creating nodes
	shared_ptr<ASTNode> nodeToExtractFrom(new ProgramASTNode(programToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(procedureNameToken));
	Entity procedureEntity = Entity::createProcedureEntity(procedureNameToken);

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTNode(stmtLstToken));

	shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
	xNode->setLineNumber(2);
	Entity xEntity = Entity::createVariableEntity(2, xToken);

	shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constant));
	constantNode->setLineNumber(2);
	Entity constantEntity = Entity::createConstantEntity(2, constant);


	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
	assignNode->setLineNumber(2);
	Entity assignEntity = Entity::createAssignEntity(2);

	shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
	yNode->setLineNumber(1);
	Entity yEntity = Entity::createVariableEntity(1, yToken);


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
	readNode->setLineNumber(1);
	Entity readEntity = Entity::createReadEntity(1);


	assignNode->addChild(xNode);
	assignNode->addChild(constantNode);

	readNode->addChild(yNode);

	stmtLstNode->addChild(readNode);
	stmtLstNode->addChild(assignNode);

	procedureNode->addChild(stmtLstNode);

	nodeToExtractFrom->addChild(procedureNode);
	
	// Creating relationships
	Relationship procedureModifiesy = Relationship::createModifiesRelationship(procedureEntity, yEntity);
	Relationship procedureModifiesx = Relationship::createModifiesRelationship(procedureEntity, xEntity);
	Relationship readModifiesY = Relationship::createModifiesRelationship(readEntity, yEntity);
	Relationship assignModifiesX = Relationship::createModifiesRelationship(assignEntity, xEntity);



	vector<Relationship> expectedRelation = vector<Relationship>{ procedureModifiesy, procedureModifiesx, readModifiesY, assignModifiesX };

	test(nodeToExtractFrom, expectedRelation);
}