#include "catch.hpp"

#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesStarRelationship.h>
#include <pkb/PkbException.h>
#include <pkb/PKB.h>
#include <pkb/PKB.cpp>

#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>

#include <memory>
#include <string>
using namespace std;

TEST_CASE("Test add and get procedure") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;
		shared_ptr<PKBUpdateHandler> pu = pkb.getUpdateHandler();
		shared_ptr<PKBQueryHandler> pq = pkb.getQueryHandler();

		// when 
		pu->addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pq->retrieveAllProcedureEntities();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(expectedEntities[i] == all[i]);
		}
	};

	SECTION("Add a bunch of procedures") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateProcedure("procedure1"),
			PQLEntity::generateProcedure("procedure2"),
			PQLEntity::generateProcedure("procedure3")
		};
		vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity(EntityType::PROCEDURE, INVALID_LINE_NUMBER, Token("procedure1", TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::PROCEDURE, INVALID_LINE_NUMBER, Token("procedure2", TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::PROCEDURE, INVALID_LINE_NUMBER, Token("procedure3", TokenType::NAME_OR_KEYWORD))
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test add and get variables") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;
		shared_ptr<PKBUpdateHandler> pu = pkb.getUpdateHandler();
		shared_ptr<PKBQueryHandler> pq = pkb.getQueryHandler();

		// when 
		pu->addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pq->retrieveAllVariables();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(expectedEntities[i] == all[i]);
		}
	};

	SECTION("Add a bunch of variable") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateProcedure("v1"),
			PQLEntity::generateProcedure("v2"),
			PQLEntity::generateProcedure("v3")
		};
		vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity(EntityType::VARIABLE, INVALID_LINE_NUMBER, Token("v1", TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::VARIABLE, INVALID_LINE_NUMBER, Token("v2", TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::VARIABLE, INVALID_LINE_NUMBER, Token("v3", TokenType::NAME_OR_KEYWORD))
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test add and get statements") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;
		shared_ptr<PKBUpdateHandler> pu = pkb.getUpdateHandler();
		shared_ptr<PKBQueryHandler> pq = pkb.getQueryHandler();

		// when 
		pu->addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pq->retrieveAllStatementEntities();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(expectedEntities[i] == all[i]);
		}
	};

	SECTION("One of each statement type") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(1),
			PQLEntity::generateStatement(2),
			PQLEntity::generateStatement(3),
			PQLEntity::generateStatement(4),
			PQLEntity::generateStatement(5),
			PQLEntity::generateStatement(6)
		};
		vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity(EntityType::READ, 1, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::PRINT, 2, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::ASSIGN, 3, Token(EQUAL_OPERATOR, TokenType::OPERATOR)),
			Entity(EntityType::CALL, 4, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::WHILE, 5, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::IF, 6, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD))
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test retrieve statements by type") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd, PKBTrackedStatementType pkbTrackedStatementType) {
		// given
		PKB pkb;
		shared_ptr<PKBUpdateHandler> pu = pkb.getUpdateHandler();
		shared_ptr<PKBQueryHandler> pq = pkb.getQueryHandler();

		// when 
		pu->addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pq->retrieveStatementEntitiesByType(pkbTrackedStatementType);
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(expectedEntities[i] == all[i]);
		}
	};

	// shared statements
	vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity(EntityType::READ, 1, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::PRINT, 2, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::ASSIGN, 3, Token(EQUAL_OPERATOR, TokenType::OPERATOR)),
			Entity(EntityType::CALL, 4, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::WHILE, 5, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD)),
			Entity(EntityType::IF, 6, Token(INVALID_IDENTIFIER, TokenType::NAME_OR_KEYWORD))
	};

	// tests
	SECTION("One of each statement type, all") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(1),
			PQLEntity::generateStatement(2),
			PQLEntity::generateStatement(3),
			PQLEntity::generateStatement(4),
			PQLEntity::generateStatement(5),
			PQLEntity::generateStatement(6)
		};

		test(expectedEntities, toAdd, PKBTrackedStatementType::ALL);
	};
	SECTION("One of each statement type, read") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(1),
		};

		test(expectedEntities, toAdd, PKBTrackedStatementType::READ);
	};
	SECTION("One of each statement type, print") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(2)
		};

		test(expectedEntities, toAdd, PKBTrackedStatementType::PRINT);
	};
	SECTION("One of each statement type, assign") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(3)
		};

		test(expectedEntities, toAdd, PKBTrackedStatementType::ASSIGN);
	};
	SECTION("One of each statement type, call") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(4)
		};

		test(expectedEntities, toAdd, PKBTrackedStatementType::ASSIGN);
	};
	SECTION("One of each statement type, while") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(5)
		};
		test(expectedEntities, toAdd, PKBTrackedStatementType::WHILE);
	};
	SECTION("One of each statement type, all") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateStatement(6)
		};

		test(expectedEntities, toAdd, PKBTrackedStatementType::IF);
	};
}