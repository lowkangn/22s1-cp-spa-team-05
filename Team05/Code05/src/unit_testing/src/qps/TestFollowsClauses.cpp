#include "catch.hpp"
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/FollowsTClause.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("FollowsClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		FollowsClause followsClause = FollowsClause(lhs, rhs);

		// when
		bool actual = followsClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> followsClauseAgain(new FollowsClause(firstStmtArg, secondStmtArg));
		testEquals(followsClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> followsClauseDiffLhs(new FollowsClause(secondStmtArg, secondStmtArg));
		testEquals(followsClauseDiffLhs, false);

		shared_ptr<RelationshipClause> followsClauseDiffRhs(new FollowsClause(firstStmtArg, firstStmtArg));
		testEquals(followsClauseDiffRhs, false);

		shared_ptr<RelationshipClause> followsClauseDiffArgType(new FollowsClause(firstStmtArg, lineArg));
		testEquals(followsClauseDiffArgType, false);
	}

	SECTION("Not equal, FollowsClause vs FollowsTClause") {
		shared_ptr<RelationshipClause> followsTClause(new FollowsTClause(firstStmtArg, secondStmtArg));
		testEquals(followsTClause, false);
	}
}

TEST_CASE("FollowsTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		FollowsTClause followsTClause = FollowsTClause(lhs, rhs);

		// when
		bool actual = followsTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> followsTClauseAgain(new FollowsTClause(firstStmtArg, secondStmtArg));
		testEquals(followsTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> followsTClauseDiffLhs(new FollowsTClause(secondStmtArg, secondStmtArg));
		testEquals(followsTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> followsTClauseDiffRhs(new FollowsTClause(firstStmtArg, firstStmtArg));
		testEquals(followsTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> followsTClauseDiffArgType(new FollowsTClause(firstStmtArg, lineArg));
		testEquals(followsTClauseDiffArgType, false);
	}

	SECTION("Not equal, FollowsClause vs FollowsTClause") {
		shared_ptr<RelationshipClause> followsClause(new FollowsClause(firstStmtArg, secondStmtArg));
		testEquals(followsClause, false);
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

	// Initialise follows relationships
	Relationship followsReadA2 = Relationship::createFollowsRelationship(readEntity, a2Entity);
	Relationship followsA2While = Relationship::createFollowsRelationship(a2Entity, whileEntity);
	Relationship followsPrintA5 = Relationship::createFollowsRelationship(printEntity, a5Entity);
	Relationship followsWhileIf = Relationship::createFollowsRelationship(whileEntity, ifEntity);

	Relationship followsStarReadA2 = Relationship::createFollowsTRelationship(readEntity, a2Entity);
	Relationship followsStarReadWhile = Relationship::createFollowsTRelationship(readEntity, whileEntity);
	Relationship followsStarReadIf = Relationship::createFollowsTRelationship(readEntity, ifEntity);

	Relationship followsStarA2While = Relationship::createFollowsTRelationship(a2Entity, whileEntity);
	Relationship followsStarA2If = Relationship::createFollowsTRelationship(a2Entity, ifEntity);

	Relationship followsStarWhileIf = Relationship::createFollowsTRelationship(whileEntity, ifEntity);

	Relationship followsStarPrintA5 = Relationship::createFollowsTRelationship(printEntity, a5Entity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlA5 = PQLEntity::generateStatement(5);
	PQLEntity pqlI6 = PQLEntity::generateStatement(6);

	PQLRelationship pqlFollowsR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsP4A5 = PQLRelationship(pqlP4, pqlA5);
	PQLRelationship pqlFollowsW3I6 = PQLRelationship(pqlW3, pqlI6);

	PQLRelationship pqlFollowsStarR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsStarR1W3 = PQLRelationship(pqlR1, pqlW3);
	PQLRelationship pqlFollowsStarR1I6 = PQLRelationship(pqlR1, pqlI6);

	PQLRelationship pqlFollowsStarA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsStarA2I6 = PQLRelationship(pqlA2, pqlI6);

	PQLRelationship pqlFollowsStarW3I6 = PQLRelationship(pqlW3, pqlI6);

	PQLRelationship pqlFollowsStarP4A5 = PQLRelationship(pqlP4, pqlA5);

	// Clause Arguments
	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument readArg = ClauseArgument::createReadArg("rr");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");
	ClauseArgument ifArg = ClauseArgument::createIfArg("if");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("while");
	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineTwoArg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument lineFourArg = ClauseArgument::createLineNumberArg("4");
	ClauseArgument lineFiveArg = ClauseArgument::createLineNumberArg("5");
	ClauseArgument lineSixArg = ClauseArgument::createLineNumberArg("6");
};

TEST_CASE("FollowsClause: test execute") {
	auto testExecute = [](FollowsClause followsClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = followsClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, a5Entity, ifEntity };
	vector<Relationship> relationships{ followsReadA2, followsA2While, followsPrintA5, followsWhileIf, followsStarReadA2, 
		followsStarReadWhile, followsStarReadIf, followsStarA2While, followsStarA2If, followsStarWhileIf, followsStarPrintA5 };
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	FollowsClause clause = FollowsClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlFollowsR1A2, pqlFollowsA2W3, pqlFollowsP4A5, pqlFollowsW3I6 };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);

	SECTION("Two stmt synoynms - all follows results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = FollowsClause(readArg, assignArg);
		expectedRetrievedFromPkb = { pqlFollowsR1A2 };
		expectedClauseResult = RelationshipClauseResult(readArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(printArg, assignArg);
		expectedRetrievedFromPkb = { pqlFollowsP4A5 };
		expectedClauseResult = RelationshipClauseResult(printArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(whileArg, ifArg);
		expectedRetrievedFromPkb = { pqlFollowsW3I6 };
		expectedClauseResult = RelationshipClauseResult(whileArg, ifArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
		expectedRetrievedFromPkb = {};

		clause = FollowsClause(assignArg, assignArg);
		expectedClauseResult = RelationshipClauseResult(assignArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(ifArg, firstStmtArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, firstStmtArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - non empty results") {
		clause = FollowsClause(lineTwoArg, whileArg);
		expectedRetrievedFromPkb = { pqlFollowsA2W3 };
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, whileArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(printArg, lineFiveArg);
		expectedRetrievedFromPkb = { pqlFollowsP4A5 };
		expectedClauseResult = RelationshipClauseResult(printArg, lineFiveArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - empty results") {
		expectedRetrievedFromPkb = {};

		clause = FollowsClause(lineOneArg, readArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(whileArg, lineFourArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - non empty results") {
		clause = FollowsClause(lineOneArg, lineTwoArg);
		expectedRetrievedFromPkb = { pqlFollowsR1A2 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(lineFourArg, lineFiveArg);
		expectedRetrievedFromPkb = { pqlFollowsP4A5 };
		expectedClauseResult = RelationshipClauseResult(lineFourArg, lineFiveArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = FollowsClause(lineOneArg, lineSixArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsClause(lineFiveArg, lineFourArg);
		expectedClauseResult = RelationshipClauseResult(lineFiveArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

}

TEST_CASE("FollowsTClause: test execute") {
	auto testExecute = [](FollowsTClause followsTClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = followsTClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, a5Entity, ifEntity };
	vector<Relationship> relationships{ followsReadA2, followsA2While, followsPrintA5, followsWhileIf, followsStarReadA2, 
		followsStarReadWhile, followsStarReadIf, followsStarA2While, followsStarA2If,followsStarWhileIf, followsStarPrintA5 };
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	FollowsTClause clause = FollowsTClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlFollowsStarR1A2, pqlFollowsStarR1W3, pqlFollowsStarR1I6, 
		pqlFollowsStarA2W3, pqlFollowsStarA2I6, pqlFollowsStarW3I6, pqlFollowsStarP4A5 };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);

	SECTION("Two stmt synoynms - all followsStar results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = FollowsTClause(readArg, whileArg);
		expectedRetrievedFromPkb = { pqlFollowsStarR1W3 };
		expectedClauseResult = RelationshipClauseResult(readArg, whileArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(firstStmtArg, assignArg);
		expectedRetrievedFromPkb = { pqlFollowsStarR1A2, pqlFollowsStarP4A5 };
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(readArg, firstStmtArg);
		expectedRetrievedFromPkb = { pqlFollowsStarR1A2, pqlFollowsStarR1W3, pqlFollowsStarR1I6 };
		expectedClauseResult = RelationshipClauseResult(readArg, firstStmtArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
		expectedRetrievedFromPkb = {};

		clause = FollowsTClause(ifArg, assignArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(firstStmtArg, readArg);
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(assignArg, printArg);
		expectedClauseResult = RelationshipClauseResult(assignArg, printArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - non empty results") {
		clause = FollowsTClause(lineOneArg, whileArg);
		expectedRetrievedFromPkb = { pqlFollowsStarR1W3 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, whileArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(assignArg, lineSixArg);
		expectedRetrievedFromPkb = { pqlFollowsStarA2I6 };
		expectedClauseResult = RelationshipClauseResult(assignArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - empty results") {
		expectedRetrievedFromPkb = {};

		clause = FollowsTClause(lineOneArg, readArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(whileArg, lineFourArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - non empty results") {
		clause = FollowsTClause(lineOneArg, lineTwoArg);
		expectedRetrievedFromPkb = { pqlFollowsStarR1A2 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(lineOneArg, lineSixArg);
		expectedRetrievedFromPkb = { pqlFollowsStarR1I6 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = FollowsTClause(lineFourArg, lineSixArg);
		expectedClauseResult = RelationshipClauseResult(lineFourArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = FollowsTClause(lineTwoArg, lineFourArg);
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

}
