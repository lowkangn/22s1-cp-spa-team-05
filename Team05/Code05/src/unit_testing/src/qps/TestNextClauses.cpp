#include "catch.hpp"
#include <qps/query/clause/NextClause.h>
#include <qps/query/clause/NextTClause.h>
#include <pkb/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/design_objects/relationships/PkbNextStarRelationship.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("NextClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		NextClause nextClause = NextClause(lhs, rhs);

		// when
		bool actual = nextClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> nextClauseAgain(new NextClause(firstStmtArg, secondStmtArg));
		testEquals(nextClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> nextClauseDiffLhs(new NextClause(secondStmtArg, secondStmtArg));
		testEquals(nextClauseDiffLhs, false);

		shared_ptr<RelationshipClause> nextClauseDiffRhs(new NextClause(firstStmtArg, firstStmtArg));
		testEquals(nextClauseDiffRhs, false);

		shared_ptr<RelationshipClause> nextClauseDiffArgType(new NextClause(firstStmtArg, lineArg));
		testEquals(nextClauseDiffArgType, false);
	}

	SECTION("Not equal, NextClause vs NextTClause") {
		shared_ptr<RelationshipClause> nextTClause(new NextTClause(firstStmtArg, secondStmtArg));
		testEquals(nextTClause, false);
	}
}

TEST_CASE("NextTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		NextTClause nextTClause = NextTClause(lhs, rhs);

		// when
		bool actual = nextTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> nextTClauseAgain(new NextTClause(firstStmtArg, secondStmtArg));
		testEquals(nextTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> nextTClauseDiffLhs(new NextTClause(secondStmtArg, secondStmtArg));
		testEquals(nextTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> nextTClauseDiffRhs(new NextTClause(firstStmtArg, firstStmtArg));
		testEquals(nextTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> nextTClauseDiffArgType(new NextTClause(firstStmtArg, lineArg));
		testEquals(nextTClauseDiffArgType, false);
	}

	SECTION("Not equal, NextClause vs NextTClause") {
		shared_ptr<RelationshipClause> nextClause(new NextClause(firstStmtArg, secondStmtArg));
		testEquals(nextClause, false);
	}
}

// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
			1. read y;
			2. x = y;
			3. while (x > 0) {
			4.	   print z;
			5.     x = x - 1;
			   }
			6. if (y < 5) then {
			7.     y = 3;
			   } else {
			8.     y = z;
			   }
		}
	*/

	// Initialise statement entities
	Entity readEntity = Entity::createReadEntity(1);
	Entity a2Entity = Entity::createAssignEntity(2);
	Entity whileEntity = Entity::createWhileEntity(3);
	Entity printEntity = Entity::createPrintEntity(4);
	Entity a5Entity = Entity::createAssignEntity(5);
	Entity ifEntity = Entity::createIfEntity(6);
	Entity a7Entity = Entity::createAssignEntity(7);
	Entity a8Entity = Entity::createAssignEntity(8);

	// Initialise next relationships
	Relationship nextReadA2 = Relationship::createNextRelationship(readEntity, a2Entity);
	Relationship nextA2While = Relationship::createNextRelationship(a2Entity, whileEntity);
	Relationship nextWhilePrint = Relationship::createNextRelationship(whileEntity, printEntity);
	Relationship nextPrintA5 = Relationship::createNextRelationship(printEntity, a5Entity);
	Relationship nextA5While = Relationship::createNextRelationship(a5Entity, whileEntity);
	Relationship nextWhileIf = Relationship::createNextRelationship(whileEntity, ifEntity);
	Relationship nextIfA7 = Relationship::createNextRelationship(ifEntity, a7Entity);
	Relationship nextIfA8 = Relationship::createNextRelationship(ifEntity, a8Entity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlA5 = PQLEntity::generateStatement(5);
	PQLEntity pqlI6 = PQLEntity::generateStatement(6);
	PQLEntity pqlA7 = PQLEntity::generateStatement(7);
	PQLEntity pqlA8 = PQLEntity::generateStatement(8);
	
	PQLRelationship pqlNextR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlNextA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlNextW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlNextP4A5 = PQLRelationship(pqlP4, pqlA5);
	PQLRelationship pqlNextA5W3 = PQLRelationship(pqlA5, pqlW3);
	PQLRelationship pqlNextW3I6 = PQLRelationship(pqlW3, pqlI6);
	PQLRelationship pqlNextI6A7 = PQLRelationship(pqlI6, pqlA7);
	PQLRelationship pqlNextI6A8 = PQLRelationship(pqlI6, pqlA8);

	// Clause Arguments
	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument readArg = ClauseArgument::createReadArg("rr");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");
	ClauseArgument ifArg = ClauseArgument::createIfArg("if");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("while");

	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineTwoArg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument lineFourArg = ClauseArgument::createLineNumberArg("4");
	ClauseArgument lineFiveArg = ClauseArgument::createLineNumberArg("5");
	ClauseArgument lineSixArg = ClauseArgument::createLineNumberArg("6");

	TEST_CASE("NextClause: test execute") {
		auto testExecute = [](NextClause nextClause,
			RelationshipClauseResult expectedClauseResult,
			shared_ptr<PKB> pkb) {
				// given
				shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

				// when
				shared_ptr<RelationshipClauseResult> resPtr = nextClause.execute(pkbInterface);
				RelationshipClauseResult actualClauseResult = *resPtr;

				// then
				REQUIRE(actualClauseResult == expectedClauseResult);

		};


		// ------ PKB ------ 
		shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
		vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, a5Entity, ifEntity, a7Entity, a8Entity };
		vector<Relationship> relationships{ nextReadA2, nextA2While, nextWhilePrint, nextPrintA5, nextA5While, nextWhileIf,
				nextIfA7, nextIfA8 };
		pkb->addEntities(entities);
		pkb->addRelationships(relationships);


		// ------ QPS ------ 
		NextClause clause = NextClause(firstStmtArg, secondStmtArg);
		vector<PQLRelationship> expectedRetrievedFromPkb = { pqlNextR1A2, pqlNextA2W3, pqlNextW3P4, pqlNextP4A5, pqlNextA5W3, 
				pqlNextW3I6, pqlNextI6A7, pqlNextI6A8 };
		RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
			expectedRetrievedFromPkb);

		SECTION("Two stmt synoynms - all next results") {
			testExecute(clause, expectedClauseResult, pkb);
		}

		SECTION("Other stmtRef synonyms - non empty results") {
			clause = NextClause(readArg, assignArg);
			expectedRetrievedFromPkb = { pqlNextR1A2 };
			expectedClauseResult = RelationshipClauseResult(readArg, assignArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(printArg, assignArg);
			expectedRetrievedFromPkb = { pqlNextP4A5 };
			expectedClauseResult = RelationshipClauseResult(printArg, assignArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(ifArg, assignArg);
			expectedRetrievedFromPkb = { pqlNextI6A7, pqlNextI6A8 };
			expectedClauseResult = RelationshipClauseResult(ifArg, assignArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);
		}

		SECTION("Other stmtRef synonyms - empty results") {
			expectedRetrievedFromPkb = {};

			clause = NextClause(assignArg, assignArg);
			expectedClauseResult = RelationshipClauseResult(assignArg, assignArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(whileArg, readArg);
			expectedClauseResult = RelationshipClauseResult(whileArg, readArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);
		}

		SECTION("One line number and one stmtRef - non empty results") {
			clause = NextClause(lineTwoArg, whileArg);
			expectedRetrievedFromPkb = { pqlNextA2W3 };
			expectedClauseResult = RelationshipClauseResult(lineTwoArg, whileArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(whileArg, lineFourArg);
			expectedRetrievedFromPkb = { pqlNextW3P4 };
			expectedClauseResult = RelationshipClauseResult(whileArg, lineFourArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);
		}

		SECTION("One line number and one stmtRef - empty results") {
			expectedRetrievedFromPkb = {};

			clause = NextClause(lineOneArg, readArg);
			expectedClauseResult = RelationshipClauseResult(lineOneArg, readArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(whileArg, lineFiveArg);
			expectedClauseResult = RelationshipClauseResult(whileArg, lineFiveArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);
		}

		SECTION("Two line numbers - non empty results") {
			clause = NextClause(lineOneArg, lineTwoArg);
			expectedRetrievedFromPkb = { pqlNextR1A2 };
			expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(lineFourArg, lineFiveArg);
			expectedRetrievedFromPkb = { pqlNextP4A5 };
			expectedClauseResult = RelationshipClauseResult(lineFourArg, lineFiveArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);
		}

		SECTION("Two line numbers - empty results") {
			expectedRetrievedFromPkb = {};

			clause = NextClause(lineOneArg, lineSixArg);
			expectedClauseResult = RelationshipClauseResult(lineOneArg, lineSixArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);

			clause = NextClause(lineFiveArg, lineFourArg);
			expectedClauseResult = RelationshipClauseResult(lineFiveArg, lineFourArg, expectedRetrievedFromPkb);
			testExecute(clause, expectedClauseResult, pkb);
		}
	}
};
