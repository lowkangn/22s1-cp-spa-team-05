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
#include<pkb/table_managers/PkbPatternTable.cpp>
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
		vector<Entity> toAdd = {
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("procedure1")),
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("procedure2")),
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("procedure3")),
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
		vector<Entity> toAdd = {
			Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("v1")),
			Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("v2")),
			Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("v3")),
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
		vector<Entity> toAdd = {
			Entity::createConstantEntity(INVALID_LINE_NUMBER, Token::createIntegerToken("1")),
			Entity::createConstantEntity(INVALID_LINE_NUMBER, Token::createIntegerToken("2")),
			Entity::createConstantEntity(INVALID_LINE_NUMBER, Token::createIntegerToken("3")),
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
		vector<Entity> toAdd = {
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
		vector<Relationship> toAdd = {
			Relationship::createFollowsRelationship(statement1, statement2),
			Relationship::createFollowsRelationship(statement2, statement3),
			Relationship::createFollowsRelationship(statement3,statement4),

		};

		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one a specific line
		lhs = ClauseArgument::createLineNumberArg("1");
		 rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);

		// test 4: both different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
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
		vector<Relationship> toAdd = {
			Relationship::createFollowsTRelationship(statement1, statement2),
			Relationship::createFollowsTRelationship(statement2, statement3),
			Relationship::createFollowsTRelationship(statement3, statement4),
			Relationship::createFollowsTRelationship(statement1, statement3),
			Relationship::createFollowsTRelationship(statement1, statement4),
			Relationship::createFollowsTRelationship(statement2, statement4)
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
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
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
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
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: both different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
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
		vector<Relationship> toAdd = {
			Relationship::createParentRelationship(statement1, statement2),
			Relationship::createParentRelationship(statement3, statement4),
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 3: both statement synonyms, but different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one line number
		lhs = ClauseArgument::createLineNumberArg("3");
		rhs = ClauseArgument::createStmtArg("s");
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
		vector<Relationship> toAdd = {
			Relationship::createParentTRelationship(statement1, statement2),
			Relationship::createParentTRelationship(statement3, statement4),
			Relationship::createParentTRelationship(statement1, statement4),
			Relationship::createParentTRelationship(statement1, statement3),
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult3),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult3),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: both statement synonyms, but different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one line number
		lhs = ClauseArgument::createLineNumberArg("3");
		rhs = ClauseArgument::createStmtArg("s");
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
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createPrintEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);

		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createUsesRelationship(statement1, x),
			Relationship::createUsesRelationship(statement2, x),
			Relationship::createUsesRelationship(statement3, x),
			Relationship::createUsesRelationship(procedure, x),
			Relationship::createUsesRelationship(statement4, x),
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: assign and variable
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 2: print and variable
		lhs = ClauseArgument::createPrintArg("pn");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult2, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 3a: if and variable
		lhs = ClauseArgument::createIfArg("ifs");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult3, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 3b: while and variable
		lhs = ClauseArgument::createWhileArg("w");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 4: procedure and variable
		lhs = ClauseArgument::createProcedureArg("p");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 5: call and variable
		lhs = ClauseArgument::createCallArg("c");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult4, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 6: rhs wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createVariableArg("v");
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
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createAssignEntity(4);
		Entity statement5 = Entity::createCallEntity(5);
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createModifiesRelationship(statement1, x),
			Relationship::createModifiesRelationship(statement2, x),
			Relationship::createModifiesRelationship(statement3, x),
			Relationship::createModifiesRelationship(procedure, x),
			Relationship::createModifiesRelationship(statement5, x),
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
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 2: print and variable
		lhs = ClauseArgument::createReadArg("r");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult2, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);


		// test 3a: if and variable
		lhs = ClauseArgument::createIfArg("ifs");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult3, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 3b: while and variable
		lhs = ClauseArgument::createWhileArg("w");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {

		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 4: procedure and variable
		lhs = ClauseArgument::createProcedureArg("p");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 5: call and variable
		lhs = ClauseArgument::createCallArg("c");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult5, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 6: rhs wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createVariableArg("v");
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

TEST_CASE("Test add and get patterns") {
	auto test = [](PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLPattern> expectedPatterns, vector<Pattern> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addPatterns(toAdd);

		// then 
		vector<PQLPattern> all = pkb.retrievePatterns(statementType, lhs, rhs);
		REQUIRE(expectedPatterns.size() == all.size());
		for (PQLPattern e : expectedPatterns) {
			bool found = false;
			for (PQLPattern p : all) {
				if (p == e) {
					found = true;
					break;
				}
			}
			if (!found) {
				for (PQLPattern p : all) {
					cout << p.toString() << endl;
				}
				cout << e.toString() << endl;
			}
			REQUIRE(found);
		}
	};

	// some patterns to add
	/*
		x = 2 * x + y // is x, 2x*y+ in postfix
		y = 3 / y - 2 // is 3y/2- in postfix
		z = x + y // is xy+ in postfix
		z = x // is x in postfix
		y = x + y // is xy+ in postfix
	*/
	Pattern p1 = Pattern::createAssignPattern(1, "x", "2x*y+");
	Pattern p2 = Pattern::createAssignPattern(2, "y", "3y/2-");
	Pattern p3 = Pattern::createAssignPattern(3, "z", "xy+");
	Pattern p4 = Pattern::createAssignPattern(4, "z", "x");
	Pattern p5 = Pattern::createAssignPattern(5, "y", "xy+");
	vector<Pattern> toAdd = {
		p1, p2, p3, p4, p5
	};

	SECTION("lhs and rhs wildcard should get all") {
		// lhs wildcard, // rhs also
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(1, "x"),
			PQLPattern::generateAssignPattern(2, "y"),
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(4, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("lhs wildcard, rhs specific") {
		// lhs wildcard, // rhs specific
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createStringLiteralArg("xy+");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("lhs specific, rhs sandwich wilcard") {
		// lhs specific, rhs sandwich wildcard _x_
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createStringWithWildCardsArg("_x_");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(1, "x"),
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(4, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Synonyms should behave like wildcards") {
		// lhs synonym, rhs specific
		ClauseArgument lhs = ClauseArgument::createVariableArg("v");
		ClauseArgument rhs = ClauseArgument::createStringLiteralArg("xy+");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

}