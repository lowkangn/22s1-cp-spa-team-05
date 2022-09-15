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
#include<pkb/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>
#include <pkb/PKB.h>

#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>

#include <algorithm>
#include <memory>
#include <string>
using namespace std;

TEST_CASE("Test add and get procedure") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pkb.retrieveAllProcedureEntities();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedEntities[i]) != all.end());
		}
	};

	SECTION("Add a bunch of procedures") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateProcedure("procedure1"),
			PQLEntity::generateProcedure("procedure2"),
			PQLEntity::generateProcedure("procedure3")
		};
		vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity::createProcedureEntity(Token("procedure1", TokenType::NAME_OR_KEYWORD)),
			Entity::createProcedureEntity(Token("procedure2", TokenType::NAME_OR_KEYWORD)),
			Entity::createProcedureEntity(Token("procedure3", TokenType::NAME_OR_KEYWORD)),
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test add and get variables") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pkb.retrieveAllVariables();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedEntities[i]) != all.end());
		}
	};

	SECTION("Add a bunch of variable") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateVariable("v1"),
			PQLEntity::generateVariable("v2"),
			PQLEntity::generateVariable("v3")
		};
		vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity::createVariableEntity(INVALID_LINE_NUMBER, Token("v1", TokenType::NAME_OR_KEYWORD)),
			Entity::createVariableEntity(INVALID_LINE_NUMBER, Token("v2", TokenType::NAME_OR_KEYWORD)),
			Entity::createVariableEntity(INVALID_LINE_NUMBER, Token("v3", TokenType::NAME_OR_KEYWORD)),
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test add and get constants") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pkb.retrieveAllConstants();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedEntities[i]) != all.end());
		}
	};

	SECTION("Add a bunch of variable") {
		vector<PQLEntity> expectedEntities = {
			PQLEntity::generateConstant(1),
			PQLEntity::generateConstant(2),
			PQLEntity::generateConstant(3)
		};
		vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity::createConstantEntity(INVALID_LINE_NUMBER, Token("1", TokenType::INTEGER)),
			Entity::createConstantEntity(INVALID_LINE_NUMBER, Token("2", TokenType::INTEGER)),
			Entity::createConstantEntity(INVALID_LINE_NUMBER, Token("3", TokenType::INTEGER)),
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test add and get statements") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pkb.retrieveAllStatementEntities();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedEntities[i]) != all.end());
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
			Entity::createReadEntity(1),
			Entity::createPrintEntity(2),
			Entity::createAssignEntity(3),
			Entity::createCallEntity(4),
			Entity::createWhileEntity(5),
			Entity::createIfEntity(6)
		};
		test(expectedEntities, toAdd);
	}
}

TEST_CASE("Test retrieve statements by type") {
	auto test = [](vector<PQLEntity> expectedEntities, vector<Entity> toAdd, PKBTrackedStatementType pkbTrackedStatementType) {
		// given
		PKB pkb;

		// when 
		pkb.addEntities(toAdd);

		// then 
		vector<PQLEntity> all = pkb.retrieveStatementEntitiesByType(pkbTrackedStatementType);
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedEntities[i]) != all.end());
		}
	};

	// shared statements
	vector<Entity> toAdd = { // TODO: entity should use factory methods!
			Entity::createReadEntity(1),
			Entity::createPrintEntity(2),
			Entity::createAssignEntity(3),
			Entity::createCallEntity(4),
			Entity::createWhileEntity(5),
			Entity::createIfEntity(6)
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

		test(expectedEntities, toAdd, PKBTrackedStatementType::CALL);
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

TEST_CASE("Test add and retrieve relationship by type and lhs rhs") {
	auto test = [](PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLRelationship> expectedRelationships, vector<Relationship> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addRelationships(toAdd);

		// then 
		vector<PQLRelationship> all = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);
		REQUIRE(expectedRelationships.size() == all.size());
		for (int i = 0; i < expectedRelationships.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedRelationships[i]) != all.end());
		}
	};
	
	SECTION("Follows") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createReadEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createAssignEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = { // TODO: entity should use factory methods!
			Relationship(
				statement1,
				statement2,
				RelationshipType::FOLLOWS
			),
			Relationship(
				statement2,
				statement3,
				RelationshipType::FOLLOWS
			),
			Relationship(
				statement3,
				statement4,
				RelationshipType::FOLLOWS
			),

		};

		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument("s1", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("s2", ArgumentType::STMT);
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard
		lhs = ClauseArgument("_", ArgumentType::WILDCARD);
		rhs = ClauseArgument("s2", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one a specific line
		lhs = ClauseArgument("1", ArgumentType::LINE_NUMBER);
		 rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);

		// test 4: both different types
		lhs = ClauseArgument("a", ArgumentType::ASSIGN);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);


	};
	SECTION("FollowsStar") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createReadEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createAssignEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = { // TODO: entity should use factory methods!
			Relationship(
				statement1,
				statement2,
				RelationshipType::FOLLOWST
			),
			Relationship(
				statement2,
				statement3,
				RelationshipType::FOLLOWST
			),
			Relationship(
				statement3,
				statement4,
				RelationshipType::FOLLOWST
			),
			Relationship(
				statement1,
				statement3,
				RelationshipType::FOLLOWST
			),
			Relationship(
				statement1,
				statement4,
				RelationshipType::FOLLOWST
			),
			Relationship(
				statement2,
				statement4,
				RelationshipType::FOLLOWST
			)
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument("s1", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("s2", ArgumentType::STMT);
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult2, statementResult4),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard
		lhs = ClauseArgument("_", ArgumentType::WILDCARD);
		rhs = ClauseArgument("s2", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one a specific line
		lhs = ClauseArgument("1", ArgumentType::LINE_NUMBER);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: both different types
		lhs = ClauseArgument("a", ArgumentType::ASSIGN);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);
		
	};
	SECTION("Parent") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createIfEntity(1);
		Entity statement2 = Entity::createAssignEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = { // TODO: entity should use factory methods!
			Relationship(
				statement1,
				statement2,
				RelationshipType::PARENT
			),
			Relationship(
				statement3,
				statement4,
				RelationshipType::PARENT
			),
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument("s1", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("s2", ArgumentType::STMT);
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 2: one wildcard
		lhs = ClauseArgument("_", ArgumentType::WILDCARD);
		rhs = ClauseArgument("s2", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 3: both statement synonyms, but different types
		lhs = ClauseArgument("a", ArgumentType::ASSIGN);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one line number
		lhs = ClauseArgument("3", ArgumentType::LINE_NUMBER);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);
	};
	SECTION("ParentStar") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createIfEntity(1);
		Entity statement2 = Entity::createAssignEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = { // TODO: entity should use factory methods!
			Relationship(
				statement1,
				statement2,
				RelationshipType::PARENTT
			),
			Relationship(
				statement3,
				statement4,
				RelationshipType::PARENTT
			),
			Relationship(
				statement1,
				statement4,
				RelationshipType::PARENTT
			),
			Relationship(
				statement1,
				statement3,
				RelationshipType::PARENTT
			),
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument("s1", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("s2", ArgumentType::STMT);
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult3),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 2: one wildcard
		lhs = ClauseArgument("_", ArgumentType::WILDCARD);
		rhs = ClauseArgument("s2", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult3),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: both statement synonyms, but different types
		lhs = ClauseArgument("a", ArgumentType::ASSIGN);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one line number
		lhs = ClauseArgument("3", ArgumentType::LINE_NUMBER);
		rhs = ClauseArgument("s", ArgumentType::STMT);
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);
		
	};
	SECTION("Uses") {
		/*
			procedure p {
			1.	x = x + 1;
			2.	print x;
			3.	if (x == 1) then {
			4. 		call x;
				}
			}
		*/
		Entity procedure = Entity::createProcedureEntity(Token("p", TokenType::NAME_OR_KEYWORD));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token("x", TokenType::NAME_OR_KEYWORD));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createPrintEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);

		// vector of relationships to add
		vector<Relationship> toAdd = { // TODO: entity should use factory methods!
			Relationship(
				statement1,
				x,
				RelationshipType::USES
			),
			Relationship(
				statement2,
				x,
				RelationshipType::USES
			),
			Relationship(
				statement3,
				x,
				RelationshipType::USES
			),
			Relationship(
				procedure,
				x,
				RelationshipType::USES
			),
			Relationship(
				statement4,
				x,
				RelationshipType::USES
			),
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: assign and variable
		ClauseArgument lhs = ClauseArgument("a", ArgumentType::ASSIGN);
		ClauseArgument rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 2: print and variable
		lhs = ClauseArgument("pn", ArgumentType::PRINT);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult2, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 3a: if and variable
		lhs = ClauseArgument("ifs", ArgumentType::IF);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult3, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 3b: while and variable
		lhs = ClauseArgument("w", ArgumentType::WHILE);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 4: procedure and variable
		lhs = ClauseArgument("p", ArgumentType::PROCEDURE);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 5: call and variable 
		lhs = ClauseArgument("c", ArgumentType::CALL);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult4, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 6: rhs wildcard
		lhs = ClauseArgument("_", ArgumentType::WILDCARD);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
			PQLRelationship(statementResult2, xResult),
			PQLRelationship(statementResult3, xResult),
			PQLRelationship(statementResult4, xResult),
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

	};

	SECTION("Modifies") {
		/*
			procedure p {
			1.	x = 1;
			2.	read x;
			3.	if (x == 1) then {
			4.		x = 2;	
			5.		call p; // recursive call
				}

			}
		*/
		Entity procedure = Entity::createProcedureEntity(Token("p", TokenType::NAME_OR_KEYWORD));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token("x", TokenType::NAME_OR_KEYWORD));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createAssignEntity(4);
		Entity statement5 = Entity::createCallEntity(5);
		// vector of relationships to add
		vector<Relationship> toAdd = { // TODO: entity should use factory methods!
			Relationship(
				statement1,
				x,
				RelationshipType::MODIFIES
			),
			Relationship(
				statement2,
				x,
				RelationshipType::MODIFIES
			),
			Relationship(
				statement3,
				x,
				RelationshipType::MODIFIES
			),
			Relationship(
				procedure,
				x,
				RelationshipType::MODIFIES
			),
			Relationship(
				statement5,
				x,
				RelationshipType::MODIFIES
			),
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);

		// test 1: assign and variable
		ClauseArgument lhs = ClauseArgument("a", ArgumentType::ASSIGN);
		ClauseArgument rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);
		// test 2: print and variable
		lhs = ClauseArgument("r", ArgumentType::READ);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult2, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);


		// test 3a: if and variable
		lhs = ClauseArgument("ifs", ArgumentType::IF);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult3, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 3b: while and variable
		lhs = ClauseArgument("w", ArgumentType::WHILE);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {

		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 4: procedure and variable
		lhs = ClauseArgument("p", ArgumentType::PROCEDURE);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 5: call and variable 
		lhs = ClauseArgument("c", ArgumentType::CALL);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult5, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 6: rhs wildcard
		lhs = ClauseArgument("_", ArgumentType::WILDCARD);
		rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
			PQLRelationship(statementResult2, xResult),
			PQLRelationship(statementResult3, xResult),
			PQLRelationship(statementResult5, xResult),
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);
	};
}

TEST_CASE("Test containsEntity") {
	auto test = [](vector<Entity> entitiesToAdd, Entity entityToTest, bool expected) {
		// given
		PKB pkb;

		// when 
		pkb.addEntities(entitiesToAdd);
		bool extractedBool = pkb.containsEntity(entityToTest);

		// then 
		REQUIRE(extractedBool == expected);
	};
	/*
		procedure main {
			x = 1
		}
	*/

	// Create tokens
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xTokenMain = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };

	// Create entities
	Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
	Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
	Entity assignEntity = Entity::createAssignEntity(1);
	Entity constEntity = Entity::createConstantEntity(1, constToken);

	vector<Entity> entitiesToAdd = vector<Entity>{ procedureMainEntity, xEntity, assignEntity, constEntity };

	// Test entities not in PKB
	// create tokens
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constTokenTest = Token{ "2", TokenType::INTEGER };
	Token testToken = Token{ "testprocedure", TokenType::NAME_OR_KEYWORD };

	Entity procedureTestEntity = Entity::createProcedureEntity(testToken);
	Entity yEntity = Entity::createVariableEntity(1, yToken);
	Entity constEntityTest = Entity::createConstantEntity(1, constTokenTest);

	SECTION("Test for entities inside PKB") {
		test(entitiesToAdd, procedureMainEntity, true);
		test(entitiesToAdd, xEntity, true);
		test(entitiesToAdd, assignEntity, true);
		test(entitiesToAdd, constEntity, true);
	}

	SECTION("Test for entities not in PKB") {
		test(entitiesToAdd, procedureTestEntity, false);
		test(entitiesToAdd, yEntity, false);
		test(entitiesToAdd, constEntityTest, false);
	}
}


TEST_CASE("Test containsRelationship") {
	auto test = [](vector<Relationship> relationshipsToAdd, Relationship relationshipToTest, bool expected) {
		// given
		PKB pkb;

		// when 
		pkb.addRelationships(relationshipsToAdd);
		bool extractedBool = pkb.containsRelationship(relationshipToTest);

		// then 
		REQUIRE(extractedBool == expected);
	};

	/*
			procedure main {
				1. x = 1
			}

			procedure readY {
				2. read y
			}
		*/

	// Create tokens
	// main

	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xTokenMain = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };

	// readY
	Token readYToken = Token{ "readY", TokenType::NAME_OR_KEYWORD };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };

	// tokens for full program
	Token stmtLstToken = Token::getPlaceHolderToken();
	Token rootNodeToken = Token::getPlaceHolderToken();


	// Create entities and relationships
	Entity programEntity = Entity::createProgramEntity();
	Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
	Entity procedureReadYEntity = Entity::createProcedureEntity(readYToken);

	Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
	Entity assignEntity = Entity::createAssignEntity(1);

	Entity yEntity = Entity::createVariableEntity(2, yToken);
	Entity readEntity = Entity::createReadEntity(2);

	Relationship procedureXRelationship = Relationship{ procedureMainEntity, xEntity, RelationshipType::MODIFIES };
	Relationship procedureYRelationship = Relationship{ procedureReadYEntity, yEntity, RelationshipType::MODIFIES };
	Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
	Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };

	vector <Relationship> relationshipsToAdd = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };

	SECTION("Test for relationships inside PKB") {
		test(relationshipsToAdd, procedureXRelationship, true);
		test(relationshipsToAdd, procedureYRelationship, true);
		test(relationshipsToAdd, assignRelation, true);
		test(relationshipsToAdd, readRelation, true);
	}

	SECTION("Test for relationships not in PKB") {
		// test relationships not in PKB
		Token zToken = Token{ "z", TokenType::NAME_OR_KEYWORD };
		Token testToken = Token{ "testprocedure", TokenType::NAME_OR_KEYWORD };

		Entity zEntity = Entity::createVariableEntity(3, zToken);
		Entity procedureTestEntity = Entity::createProcedureEntity(testToken);

		Relationship procedureZRelationship = Relationship{ procedureTestEntity, zEntity, RelationshipType::MODIFIES };
		test(relationshipsToAdd, procedureZRelationship, false);
	}

}
