#include "catch.hpp"
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/DesignExtractorManager.cpp>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.cpp>
#include <sp/design_extractor/Extractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/PatternExtractor.cpp>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.cpp>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.cpp>
#include <pkb/interfaces/PKBUpdateHandler.h>

TEST_CASE("DesignExtractor: test : extractEntity() && extractModifies()") {
	auto test = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntity, vector<Relationship> expectedRelationship) {

		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());

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

	shared_ptr<ASTNode> nodeToExtractFrom(new ProgramASTNode(programToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(procedureNameToken));
	Entity procedureEntity = procedureNode->extractEntity();

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(stmtLstToken));

	shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
	xNode->setLineNumber(2);
	Entity xEntity = xNode->extractEntity();

	shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constant));
	constantNode->setLineNumber(2);
	Entity constantEntity = constantNode->extractEntity();


	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
	assignNode->setLineNumber(2);
	Entity assignEntity = assignNode->extractEntity();

	shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
	yNode->setLineNumber(1);
	Entity yEntity = yNode->extractEntity();


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
	readNode->setLineNumber(1);
	Entity readEntity = readNode->extractEntity();


	assignNode->addChild(xNode);
	assignNode->addChild(constantNode);

	readNode->addChild(yNode);

	stmtLstNode->addChild(readNode);
	stmtLstNode->addChild(assignNode);

	procedureNode->addChild(stmtLstNode);

	nodeToExtractFrom->addChild(procedureNode);

	Relationship procedureModifiesy = Relationship(procedureEntity, yEntity, RelationshipType::MODIFIES);
	Relationship procedureModifiesx = Relationship(procedureEntity, xEntity, RelationshipType::MODIFIES);


	vector<Entity> expectedEntities = vector<Entity>{ procedureEntity, readEntity, yEntity, assignEntity, xEntity, constantEntity };
	vector<Relationship> expectedRelation = vector<Relationship>{ procedureModifiesy, procedureModifiesx };

	test(nodeToExtractFrom, expectedEntities, expectedRelation);
}