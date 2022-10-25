#include "catch.hpp"

#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>
#include <pkb/PKB.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
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


TEST_CASE("Test add and retrieve table relationships by type and lhs rhs") {
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

	SECTION("Calls") {
		/*
			procedure main {
				call alpha;
				call beta;
			}

			procedure alpha {
				call beta;
			}

			procedure beta {}
		*/
		Entity mainEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
		Entity alphaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("alpha"));
		Entity betaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("beta"));
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createCallsRelationship(mainEntity, alphaEntity),
			Relationship::createCallsRelationship(mainEntity, betaEntity),
			Relationship::createCallsRelationship(alphaEntity, betaEntity),
		};
		// shared, as PQLEntities
		PQLEntity mainProcedureResult = PQLEntity::generateProcedure("main");
		PQLEntity alphaProcedureResult = PQLEntity::generateProcedure("alpha");
		PQLEntity betaProcedureResult = PQLEntity::generateProcedure("beta");


		// test 1: both procedure synonyms
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p1");
		ClauseArgument rhs = ClauseArgument::createProcedureArg("p2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard, one specific procedure
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStringLiteralArg("alpha");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one wildcard, one procedure synonym
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one specific procedure
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);

		// test 5: both specific procedures
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createStringLiteralArg("alpha");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);
	};

	SECTION("CallsStar") {
		/*
			procedure main {
				call alpha;
			}

			procedure alpha {
				call beta;
			}

			procedure beta {}
		*/
		Entity mainEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
		Entity alphaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("alpha"));
		Entity betaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("beta"));
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createCallsTRelationship(mainEntity, alphaEntity),
			Relationship::createCallsTRelationship(mainEntity, betaEntity),
			Relationship::createCallsTRelationship(alphaEntity, betaEntity),

		};
		// shared, as PQLEntities
		PQLEntity mainProcedureResult = PQLEntity::generateProcedure("main");
		PQLEntity alphaProcedureResult = PQLEntity::generateProcedure("alpha");
		PQLEntity betaProcedureResult = PQLEntity::generateProcedure("beta");


		// test 1: both procedure synonyms
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p1");
		ClauseArgument rhs = ClauseArgument::createProcedureArg("p2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard, one specific procedure
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStringLiteralArg("beta");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one wildcard, one procedure synonym
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one specific procedure
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 5: both specific procedures
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createStringLiteralArg("alpha");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

	};

	SECTION("Exact queries (by statement no., by variable name)") {
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
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createStringLiteralArg("x");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};

		// test 2: none, but returns empty
		lhs = ClauseArgument::createLineNumberArg("4");
		rhs = ClauseArgument::createStringLiteralArg("x");
		expectedRelationships = {

		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);
	};

	SECTION("Next") {
		/*
			procedure p {
			1.	x = 1;
			2.	read x;
			3.	if (x == 1) then {
			4.		x = 2;
			5.		call p; // recursive call
				} else {
			6.		x = 3;
				}
			7.	while (x == 3) {
			8.		x = 2;
				}

			}
		*/
		// entity
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createAssignEntity(4);
		Entity statement5 = Entity::createCallEntity(5);
		Entity statement6 = Entity::createAssignEntity(6);
		Entity statement7 = Entity::createWhileEntity(7);
		Entity statement8 = Entity::createAssignEntity(8);

		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createNextRelationship(statement1, statement2),
			Relationship::createNextRelationship(statement2, statement3),
			Relationship::createNextRelationship(statement3, statement4), // if to inside if
			Relationship::createNextRelationship(statement4, statement5),
			Relationship::createNextRelationship(statement3, statement6), // if to else
			Relationship::createNextRelationship(statement6, statement7), // else to after
			Relationship::createNextRelationship(statement5, statement7), // if to after
			Relationship::createNextRelationship(statement7, statement8), // whlile
			Relationship::createNextRelationship(statement8, statement7), // while backedge
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);
		PQLEntity statementResult6 = PQLEntity::generateStatement(6);
		PQLEntity statementResult7 = PQLEntity::generateStatement(7);
		PQLEntity statementResult8 = PQLEntity::generateStatement(8);

		// test 1a: both exact, both inside -> one result
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);

		// test 1b: both exact, one not inside -> no result
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createLineNumberArg("5");
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);

		// test 2: both wildcard
		lhs = ClauseArgument::createWildcardArg(); // is wildcard
		rhs = ClauseArgument::createStmtArg("s"); // essentially wildcard
		expectedRelationships = { // expect all relationships
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult4, statementResult5),
			PQLRelationship(statementResult3, statementResult6),
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult7, statementResult8),
			PQLRelationship(statementResult8, statementResult7),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);



		// test 3: exact, wildcard
		lhs = ClauseArgument::createLineNumberArg("3"); // start at 3
		rhs = ClauseArgument::createWildcardArg();
		expectedRelationships = { // expect all relationships
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult3, statementResult6),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);


		// test 4: wildcard, exact
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createLineNumberArg("7");
		expectedRelationships = { // those that end at 7
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult8, statementResult7),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);



	}
}


TEST_CASE("Test retrieve relationship short circuits to empty result") {
	auto test = [](vector<Relationship> relationshipsToAdd, PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
		// given
		PKB pkb;

		// when 
		pkb.addRelationships(relationshipsToAdd);
		vector<PQLRelationship> retrieved = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);

		// then 
		REQUIRE(retrieved.size() == 0);
	};

	// shared, as if 4 statements in a block
	/*
		if ...
			assign ...
			if ...
				call ...
				call ...
	*/
	Entity statement1 = Entity::createIfEntity(1);
	Entity statement2 = Entity::createAssignEntity(2);
	Entity statement3 = Entity::createIfEntity(3);
	Entity statement4 = Entity::createCallEntity(4);
	Entity statement5 = Entity::createCallEntity(5);

	vector<Relationship> toAdd = {
		Relationship::createParentTRelationship(statement1, statement2),
		Relationship::createParentTRelationship(statement3, statement4),
		Relationship::createParentTRelationship(statement1, statement4),
		Relationship::createParentTRelationship(statement1, statement3),
		Relationship::createParentRelationship(statement1, statement2),
		Relationship::createParentRelationship(statement3, statement4),
		Relationship::createFollowsRelationship(statement4, statement5),
		Relationship::createFollowsTRelationship(statement4, statement5),
	};

	SECTION("Follows(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(a,a)") {
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createAssignArg("a");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(1,1)") {
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("1");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(if,if)") {
		ClauseArgument lhs = ClauseArgument::createIfArg("if");
		ClauseArgument rhs = ClauseArgument::createIfArg("if");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}


	SECTION("Follows(r,r)") {
		ClauseArgument lhs = ClauseArgument::createReadArg("r");
		ClauseArgument rhs = ClauseArgument::createReadArg("r");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}


	SECTION("Follows(pr,pr)") {
		ClauseArgument lhs = ClauseArgument::createPrintArg("pr");
		ClauseArgument rhs = ClauseArgument::createPrintArg("pr");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(w,w)") {
		ClauseArgument lhs = ClauseArgument::createWhileArg("w");
		ClauseArgument rhs = ClauseArgument::createWhileArg("w");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}


	SECTION("Follows*(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs);
	}

	SECTION("Parent(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::PARENT, lhs, rhs);
	}

	SECTION("Parent*(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs);
	}


	SECTION("Next(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::NEXT, lhs, rhs);
	}
	SECTION("Next(a,a)") {
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createAssignArg("a");
		test(toAdd, PKBTrackedRelationshipType::NEXT, lhs, rhs);
	}

	SECTION("Next(1,1)") {
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("1");
		test(toAdd, PKBTrackedRelationshipType::NEXT, lhs, rhs);
	}

}

/*
TEST_CASE("Add and get graph relationshpis by type and lhs and rhs") {

	auto test = [](PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLRelationship> expectedRelationships, shared_ptr<CFGNode> graphToAdd) {
		// given pkb 
		PKB pkb;

		// when add and retrieve relationships 
		pkb.addCfg(graphToAdd);
		vector<PQLRelationship> all = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);

		// then should be inside
		REQUIRE(expectedRelationships.size() == all.size());
		for (int i = 0; i < expectedRelationships.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedRelationships[i]) != all.end());
		}
	};

	SECTION("NextStar") { // TODO: consider refactor to cfgnode


		/*
		procedure p {
		1	x = 1; // assign
		2	call a; // call
		3	if (x == 1) then { // if
		4		read b; // read
			} else {
		5		print x; // print
			}
		6	while (x == 1) { // while
		7		x = 0; // assign
			}
		8	x = 1;
		}
		*/
/*
		// graph 
		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3}},
			{3, {4,5}},
			{4, {6}},
			{5, {6}},
			{6, {7,8}},
			{7, {6,8}},
			{8, {}},

		};
		unordered_map<int, shared_ptr<CFGNode>> nodeIdToNode = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, CFGNode::createCFGNode(Entity::createCallEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createIfEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createReadEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createPrintEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createWhileEntity(6))},
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
			{8, CFGNode::createCFGNode(Entity::createAssignEntity(8))},
		};

		shared_ptr<CFGNode> graph = CFGNode::createCFGFromAdjacencyList(nodeIdToNode, adjList, 1);
		/*
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");

		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);
		PQLEntity statementResult6 = PQLEntity::generateStatement(6);
		PQLEntity statementResult7 = PQLEntity::generateStatement(7);
		PQLEntity statementResult8 = PQLEntity::generateStatement(8);

		// test 1a: both exact but inside
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 1b: both exact but one not inside
		lhs = ClauseArgument::createLineNumberArg("4");
		rhs = ClauseArgument::createLineNumberArg("3");
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 2: both wild card
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			// 1 onwards
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult5),
			PQLRelationship(statementResult1, statementResult6),
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult1, statementResult8),

			// 2 onwards
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult2, statementResult4),
			PQLRelationship(statementResult2, statementResult5),
			PQLRelationship(statementResult2, statementResult6),
			PQLRelationship(statementResult2, statementResult7),
			PQLRelationship(statementResult2, statementResult8),

			// 3 onwards
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult3, statementResult5),
			PQLRelationship(statementResult3, statementResult6),
			PQLRelationship(statementResult3, statementResult7),
			PQLRelationship(statementResult3, statementResult8),

			// 4 onwards
			PQLRelationship(statementResult4, statementResult6),
			PQLRelationship(statementResult4, statementResult7),
			PQLRelationship(statementResult4, statementResult8),

			// 5 onwards
			PQLRelationship(statementResult5, statementResult6),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult5, statementResult8),

			// 6 onwards
			PQLRelationship(statementResult6, statementResult6), // while loop self loop
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult6, statementResult8),

			// 7 onwards
			PQLRelationship(statementResult7, statementResult6), // while loop self loop
			PQLRelationship(statementResult7, statementResult7), // while loop self loop
			PQLRelationship(statementResult7, statementResult8),

		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 3: exact then wildcard
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			// 1 onwards
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult5),
			PQLRelationship(statementResult1, statementResult6),
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult1, statementResult8),
		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);


		// test 4a: wildcard then exact, but exact not inside
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createLineNumberArg("1");
		expectedRelationships = {};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 4b: wildcard then exact, but exact is inside
		lhs = ClauseArgument::createStmtArg("s");
		rhs = ClauseArgument::createLineNumberArg("7");
		expectedRelationships = {
			// 1 onwards
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult2, statementResult7),
			PQLRelationship(statementResult3, statementResult7),
			PQLRelationship(statementResult4, statementResult7),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult7, statementResult7),

		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);
	}
}


*/



TEST_CASE("Test add and get patterns") {
	auto test = [](PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLPattern> expectedPatterns, vector<Pattern> toAdd) {

		// given
		PKB pkb;

		// when
		pkb.addPatterns(toAdd);

		vector<PQLPattern> all;

		vector<PQLPattern> patterns = pkb.retrievePatterns(statementType, lhs, rhs);
		for (int i = 0; i < patterns.size(); i++) {
			all.push_back(patterns[i]);
		}

		// then 
		REQUIRE(expectedPatterns.size() == all.size());
		for (PQLPattern e : expectedPatterns) {
			bool found = false;
			for (PQLPattern p : all) {
				if (p == e) {
					found = true;
					break;
				}
			}
			REQUIRE(found);
		}
	};

	// some patterns to add
	/*
		ASSIGN PATTERNS:
		x = 2 * x + y // is x, 2x*y+ in postfix
		y = 3 / y - 2 // is 3y/2- in postfix
		z = x + y // is xy+ in postfix
		z = x // is x in postfix
		y = x + y // is xy+ in postfix
		y = x + y // repeated on a different line

		IF PATTERNS:
		if (x == 1)
		if ((x == 0) && (y == 1))
		if ((x == 0) && (y == 1) || (z == 2))
		if (a != 3)

		WHILE PATTERNS:
		while (i > 0)
		while ((i > 0) || (j == 0))
		while ((i > 0) || (j == 0) && (k == 1))
	*/
	Pattern p1 = Pattern::createAssignPattern(1, " x ", " 2 x * y + ");
	Pattern p2 = Pattern::createAssignPattern(2, " y ", " 3 y / 2 - ");
	Pattern p3 = Pattern::createAssignPattern(3, " z ", " x y + ");
	Pattern p4 = Pattern::createAssignPattern(4, " z ", " x ");
	Pattern p5 = Pattern::createAssignPattern(5, " y ", " x y + ");
	Pattern p6 = Pattern::createAssignPattern(6, " y ", " x y + ");
	Pattern p7 = Pattern::createAssignPattern(7, " ab ", " 1000 ");

	// if (x == 1)
	Pattern p8 = Pattern::createIfPattern(8, "x");

	// if ((x == 0) && (y == 1))
	Pattern p9 = Pattern::createIfPattern(9, "x y");

	// if ((x == 0) && (y == 1) || (yz == 2))
	Pattern p10 = Pattern::createIfPattern(10, "x y yz");

	// if (a != 3)
	Pattern p11 = Pattern::createIfPattern(11, "a");

	// while (i > 0)
	Pattern p12 = Pattern::createWhilePattern(12, "i");

	// while ((i > 0) || (j == 0))
	Pattern p13 = Pattern::createWhilePattern(13, "i j");

	// while ((i > 0) || (j == 0) && (k == 1))
	Pattern p14 = Pattern::createWhilePattern(14, "i j jk");


	vector<Pattern> toAdd = {
		p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14
	};

	SECTION("lhs and rhs wildcard should get all assign patterns") {
		// lhs wildcard, // rhs also
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(1, "x"),
			PQLPattern::generateAssignPattern(2, "y"),
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(4, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
			PQLPattern::generateAssignPattern(7, "ab"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("lhs wildcard should get all if patterns") {
		ClauseArgument lhs = ClauseArgument::createWildcardArg();

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		// TODO for PQL: change generateAssignPattern method name to generatePattern
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(8, "x"),
			PQLPattern::generateAssignPattern(9, "x"),
			PQLPattern::generateAssignPattern(9, "y"),
			PQLPattern::generateAssignPattern(10, "x"),
			PQLPattern::generateAssignPattern(10, "y"),
			PQLPattern::generateAssignPattern(10, "yz"),
			PQLPattern::generateAssignPattern(11, "a"),
		};
		test(PKBTrackedStatementType::IF, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("lhs wildcard should get all while patterns") {
		ClauseArgument lhs = ClauseArgument::createWildcardArg();

		// rhs doesn't matter for while patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(12, "i"),
			PQLPattern::generateAssignPattern(13, "i"),
			PQLPattern::generateAssignPattern(13, "j"),
			PQLPattern::generateAssignPattern(14, "i"),
			PQLPattern::generateAssignPattern(14, "j"),
			PQLPattern::generateAssignPattern(14, "jk"),
		};
		test(PKBTrackedStatementType::WHILE, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("specific lhs variable for if patterns") {
		ClauseArgument lhs = ClauseArgument::createStringLiteralArg("x");

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();

		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(8, "x"),
			PQLPattern::generateAssignPattern(9, "x"),
			PQLPattern::generateAssignPattern(10, "x")
		};
		test(PKBTrackedStatementType::IF, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("specific lhs variable retrieved with proper exact regex match for if patterns") {
		// i.e. get if (..y..) but not if (..yz..)
		ClauseArgument lhs = ClauseArgument::createStringLiteralArg("y");

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();

		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(9, "y"),
			PQLPattern::generateAssignPattern(10, "y")
		};
		test(PKBTrackedStatementType::IF, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("specific lhs variable retrieved with proper exact regex match for while patterns") {
		// i.e. get while (..j..) but not while (..jk..)
		ClauseArgument lhs = ClauseArgument::createStringLiteralArg("j");

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();

		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(13, "j"),
			PQLPattern::generateAssignPattern(14, "j")
		};
		test(PKBTrackedStatementType::WHILE, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("lhs wildcard, rhs specific") {
		// lhs wildcard, // rhs specific
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("x y +");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("lhs specific, rhs sandwich wilcard") {
		// lhs specific, rhs sandwich wildcard _x_
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createPatternStringWithWildcardsArg("_x_");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(1, "x"),
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(4, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Synonyms should behave like wildcards") {
		// lhs synonym, rhs specific
		ClauseArgument lhs = ClauseArgument::createVariableArg("v");
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("x y +");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Can handle identical patterns on different lines") {
		// lhs synonym, rhs specific
		ClauseArgument lhs = ClauseArgument::createPatternStringArg("y");
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("x y +");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),

		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Can handle substring matching") {
		// 100 is a substring of 1000 (pattern7), but should not be matched
		ClauseArgument lhs = ClauseArgument::createPatternStringArg("ab");
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("100");
		vector<PQLPattern> expectedPatterns = {
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

}