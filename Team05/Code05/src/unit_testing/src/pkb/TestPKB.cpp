#include "catch.hpp"
#include <pkb/pkb_object/PKB.h>
#include <pkb/pkb_object/PKB.cpp>
#include <pkb/interfaces/PKBUpdateHandler.h>


TEST_CASE("PKB: test add entity") {

	auto testAddEntity = [](EntityPkbTableManager entityManagerPkb,
		vector<Entity> entities,
		ProgramKnowledgeBase pkb) {
			vector<int> expectedResult;

			vector<int> actualResult = pkb.addEntity(entities);

			for (auto& entity : entities) {
				// convert entity to PkbEntity
				int id;
				if (entity.getType() == EntityType::STMT) {
					int line = entity.getLine();
					PkbEntity pkbEntity = PkbEntity::generateStatement(entity.toString(), line);
					id = entityManagerPkb.add(pkbEntity);
					expectedResult.push_back(id);
				}
				else {
					PkbEntity pkbEntity = PkbEntity::generateVariable(entity.toString());
					id = entityManagerPkb.add(pkbEntity);
					expectedResult.push_back(id);
				}
				
			};


			REQUIRE(actualResult.size() == expectedResult.size());

			for (int i = 0; i < entities.size(); i++) {
				REQUIRE(actualResult[i] == expectedResult[i]);
			}
	};

	EntityPkbTableManager entityManager = EntityPkbTableManager();
	Token tokenX = Token("x", TokenType::NAME_OR_KEYWORD);
	EntityIdentifier identifierX = EntityIdentifier(tokenX, "x");
	Entity entityX = Entity(EntityType::VARIABLE, 4, tokenX, "x");

	Token tokenY = Token("x", TokenType::NAME_OR_KEYWORD);
	EntityIdentifier identifierY = EntityIdentifier(tokenY, "y");
	Entity entityY = Entity(EntityType::VARIABLE, 8, tokenY, "y");

	Token tokenStmtX = Token("read x", TokenType::OPERATOR);
	EntityIdentifier identifierStmtX = EntityIdentifier(tokenStmtX, "read x");
	Entity entityStmtX = Entity(EntityType::STMT, 4, tokenStmtX, "read x");

	Token tokenStmtY = Token("y = y + 1", TokenType::OPERATOR);
	EntityIdentifier identifierStmtY = EntityIdentifier(tokenStmtY, "read x");
	Entity entityStmtY = Entity(EntityType::STMT, 4, tokenStmtY, "read x");

	SECTION("Test single entity") {
		testAddEntity(entityManager, { entityX }, *ProgramKnowledgeBase::getInstance());
	}

	SECTION("Test multiple entities") {
		testAddEntity(entityManager, { entityX, entityY }, *ProgramKnowledgeBase::getInstance());
	}

	SECTION("Test combination of statements and entities") {
		testAddEntity(entityManager, { entityStmtX, entityStmtY, entityX, entityY }, *ProgramKnowledgeBase::getInstance());
	}
}

TEST_CASE("PKB Add Modifies Relationships") {
	auto testAddRelationship = [](EntityPkbTableManager entityManager,
		vector<Relationship> relationships,
		ProgramKnowledgeBase pkb) {
			vector<int> expectedResults;
			vector<int> relationshipIds = pkb.addRelationship(relationships);
			shared_ptr<ModifiesPkbTableManager> modifiesTable = pkb._getModifiesTable();
			unordered_map<int, PkbRelationship> modifiesMap = modifiesTable->getMap();

			for (int i = 0; i < relationships.size(); i++) {
				// RHS assumed to be variable
				int lhsPkbEntityId;
				Entity lhs = relationships[i].getEntities()[0];
				Entity rhs = relationships[i].getEntities()[1];
				if (lhs.getType() == EntityType::STMT) {
					lhsPkbEntityId = entityManager.add(PkbEntity::generateStatement(lhs.toString(), lhs.getLine()));
				}
				else {
					lhsPkbEntityId = entityManager.add(PkbEntity::generateVariable(lhs.toString()));
				}
				int rhsPkbEntityId = entityManager.add(PkbEntity::generateVariable(rhs.toString()));
				PkbRelationship pkbRelationship = modifiesMap.at(relationshipIds[i]);
				REQUIRE(pkbRelationship.getLhs() == lhsPkbEntityId);
				REQUIRE(pkbRelationship.getRhs() == rhsPkbEntityId);
			}
	};
	EntityPkbTableManager entityManager = EntityPkbTableManager();

	Token tokenStmtX = Token("read x", TokenType::OPERATOR);
	EntityIdentifier identifierStmtX = EntityIdentifier(tokenStmtX, "read x");
	Entity entityStmtX = Entity(EntityType::STMT, 4, tokenStmtX, "read x");

	Token tokenX = Token("x", TokenType::NAME_OR_KEYWORD);
	EntityIdentifier identifierX = EntityIdentifier(tokenX, "x");
	Entity entityX = Entity(EntityType::VARIABLE, 4, tokenX, "x");

	Token tokenStmtY = Token("read Y", TokenType::OPERATOR);
	EntityIdentifier identifierStmtY = EntityIdentifier(tokenStmtY, "read Y");
	Entity entityStmtY = Entity(EntityType::STMT, 4, tokenStmtY, "read Y");

	Token tokenY = Token("Y", TokenType::NAME_OR_KEYWORD);
	EntityIdentifier identifierY = EntityIdentifier(tokenY, "Y");
	Entity entityY = Entity(EntityType::VARIABLE, 4, tokenY, "Y");

	Relationship relationshipX = Relationship(entityStmtX, entityX, RelationshipType::MODIFIES);
	Relationship relationshipY = Relationship(entityStmtY, entityY, RelationshipType::MODIFIES);

	SECTION("Test single relationship") {
		testAddRelationship(entityManager, { relationshipX }, *ProgramKnowledgeBase::getInstance());
	}

	SECTION("Test multiple relationship") {
		testAddRelationship(entityManager, { relationshipX, relationshipY }, *ProgramKnowledgeBase::getInstance());
	}
}
