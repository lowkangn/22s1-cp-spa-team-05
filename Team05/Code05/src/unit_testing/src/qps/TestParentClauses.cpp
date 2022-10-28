#include "catch.hpp"
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ParentClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		ParentClause parentClause = ParentClause(lhs, rhs);

		// when
		bool actual = parentClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> parentClauseAgain(new ParentClause(firstStmtArg, secondStmtArg));
		testEquals(parentClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> parentClauseDiffLhs(new ParentClause(secondStmtArg, secondStmtArg));
		testEquals(parentClauseDiffLhs, false);

		shared_ptr<RelationshipClause> parentClauseDiffRhs(new ParentClause(firstStmtArg, firstStmtArg));
		testEquals(parentClauseDiffRhs, false);

		shared_ptr<RelationshipClause> parentClauseDiffArgType(new ParentClause(firstStmtArg, lineArg));
		testEquals(parentClauseDiffArgType, false);
	}

	SECTION("Not equal, ParentClause vs ParentTClause") {
		shared_ptr<RelationshipClause> parentTClause(new ParentTClause(firstStmtArg, secondStmtArg));
		testEquals(parentTClause, false);
	}
}

TEST_CASE("ParentTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		ParentTClause parentTClause = ParentTClause(lhs, rhs);

		// when
		bool actual = parentTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> parentTClauseAgain(new ParentTClause(firstStmtArg, secondStmtArg));
		testEquals(parentTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> parentTClauseDiffLhs(new ParentTClause(secondStmtArg, secondStmtArg));
		testEquals(parentTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> parentTClauseDiffRhs(new ParentTClause(firstStmtArg, firstStmtArg));
		testEquals(parentTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> parentTClauseDiffArgType(new ParentTClause(firstStmtArg, lineArg));
		testEquals(parentTClauseDiffArgType, false);
	}

	SECTION("Not equal, ParentClause vs ParentTClause") {
		shared_ptr<RelationshipClause> parentClause(new ParentClause(firstStmtArg, secondStmtArg));
		testEquals(parentClause, false);
	}
}


// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	1:		while (x > 1) {
	2:			y = x + 1;
	3:			if (x > 1) then {
	4:				read x;
				} else {
	5:				read y;
				}
			}
	6:		print x;
		}
	*/

	// Initialise statement entities
	Entity w1 = Entity::createWhileEntity(1);
	Entity a2 = Entity::createAssignEntity(2);
	Entity i3 = Entity::createIfEntity(3);
	Entity r4 = Entity::createReadEntity(4);
	Entity r5 = Entity::createReadEntity(5);
	Entity p6 = Entity::createPrintEntity(6);

	// Initialise some additional non-statement entities to check they are not returned
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity x = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("x"));
	Entity y = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("y"));
	Entity oneEntity = Entity::createConstantEntity(3, Token::createIntegerToken("1"));

	// Initialise parent relationships
	Relationship parentW1A2 = Relationship::createParentRelationship(w1, a2);
	Relationship parentW1I3 = Relationship::createParentRelationship(w1, i3);
	Relationship parentI3R4 = Relationship::createParentRelationship(i3, r4);
	Relationship parentI3R5 = Relationship::createParentRelationship(i3, r5);

	// Initialise parentStar relationships
	Relationship parentStarW1A2 = Relationship::createParentTRelationship(w1, a2);
	Relationship parentStarW1I3 = Relationship::createParentTRelationship(w1, i3);
	Relationship parentStarI3R4 = Relationship::createParentTRelationship(i3, r4);
	Relationship parentStarI3R5 = Relationship::createParentTRelationship(i3, r5);
	Relationship parentStarW1R4 = Relationship::createParentTRelationship(w1, r4);
	Relationship parentStarW1R5 = Relationship::createParentTRelationship(w1, r5);


	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlW1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlI3 = PQLEntity::generateStatement(3);
	PQLEntity pqlR4 = PQLEntity::generateStatement(4);
	PQLEntity pqlR5 = PQLEntity::generateStatement(5);
	PQLEntity pqlP6 = PQLEntity::generateStatement(6);

	PQLRelationship pqlParentW1A2 = PQLRelationship(pqlW1, pqlA2);
	PQLRelationship pqlParentW1I3 = PQLRelationship(pqlW1, pqlI3);
	PQLRelationship pqlParentI3R4 = PQLRelationship(pqlI3, pqlR4);
	PQLRelationship pqlParentI3R5 = PQLRelationship(pqlI3, pqlR5);

	PQLRelationship pqlParentStarW1A2 = PQLRelationship(pqlW1, pqlA2);
	PQLRelationship pqlParentStarW1I3 = PQLRelationship(pqlW1, pqlI3);
	PQLRelationship pqlParentStarI3R4 = PQLRelationship(pqlI3, pqlR4);
	PQLRelationship pqlParentStarI3R5 = PQLRelationship(pqlI3, pqlR5);
	PQLRelationship pqlParentStarW1R4 = PQLRelationship(pqlW1, pqlR4);
	PQLRelationship pqlParentStarW1R5 = PQLRelationship(pqlW1, pqlR5);

	// Clause Arguments
	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument ifArg = ClauseArgument::createIfArg("if");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("while");
	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineTwoArg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument lineFourArg = ClauseArgument::createLineNumberArg("4");
	ClauseArgument lineSixArg = ClauseArgument::createLineNumberArg("6");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument readArg = ClauseArgument::createReadArg("rr");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");
};

TEST_CASE("ParentClause: test execute") {
	auto testExecute = [](ParentClause parentClause, 
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = parentClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;
			
			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};
	

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ w1, a2, i3, r4, r5, p6, main, x, y, oneEntity };
	vector<Relationship> relationships{ parentW1A2, parentW1I3, parentI3R4, parentI3R5,
		parentStarW1A2, parentStarW1I3, parentStarI3R4, parentStarI3R5, parentStarW1R4, parentStarW1R5 };
	pkb -> addEntities(entities);
	pkb -> addRelationships(relationships);

	
	// ------ QPS ------ 
	ParentClause clause = ParentClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlParentW1A2, pqlParentW1I3, pqlParentI3R4, pqlParentI3R5 };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);
	SECTION("Two stmt synoynms - all parent results") {
		testExecute(clause, expectedClauseResult, pkb);
	}
	
	SECTION("Other stmtRef synonyms - non empty results") {
		clause = ParentClause(ifArg, readArg);
		expectedRetrievedFromPkb = { pqlParentI3R4, pqlParentI3R5 };
		expectedClauseResult = RelationshipClauseResult(ifArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentClause(whileArg, ifArg);
		expectedRetrievedFromPkb = { pqlParentW1I3 };
		expectedClauseResult = RelationshipClauseResult(whileArg, ifArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ParentClause(readArg, readArg);
		expectedClauseResult = RelationshipClauseResult(readArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentClause(whileArg, readArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentClause(whileArg, printArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, printArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line numbers - non empty results") {
		clause = ParentClause(lineOneArg, lineTwoArg);
		expectedRetrievedFromPkb = { pqlParentW1A2 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ParentClause(lineOneArg, lineSixArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentClause(lineOneArg, lineFourArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentClause(lineTwoArg, lineSixArg);
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

}

TEST_CASE("ParentTClause: test execute") {
	auto testExecute = [](ParentTClause parentTClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = parentTClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ w1, a2, i3, r4, r5, p6, main, x, y, oneEntity };
	vector<Relationship> relationships{
		parentW1A2,
		parentW1I3,
		parentI3R4,
		parentI3R5,
		parentStarW1A2,
		parentStarW1I3,
		parentStarI3R4,
		parentStarI3R5,
		parentStarW1R4,
		parentStarW1R5
	};
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	ParentTClause clause = ParentTClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlParentStarW1A2, pqlParentStarW1I3,
		pqlParentStarI3R4, pqlParentStarI3R5, pqlParentStarW1R4, pqlParentStarW1R5
	};
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);
	SECTION("Two stmt synoynms - all parentStar results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = ParentTClause(ifArg, readArg);
		expectedRetrievedFromPkb = { pqlParentStarI3R4, pqlParentStarI3R5 };
		expectedClauseResult = RelationshipClauseResult(ifArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(whileArg, ifArg);
		expectedRetrievedFromPkb = { pqlParentStarW1I3 };
		expectedClauseResult = RelationshipClauseResult(whileArg, ifArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(whileArg, readArg);
		expectedRetrievedFromPkb = { pqlParentStarW1R4, pqlParentStarW1R5 };
		expectedClauseResult = RelationshipClauseResult(whileArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ParentTClause(readArg, readArg);
		expectedClauseResult = RelationshipClauseResult(readArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(ifArg, printArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, printArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(whileArg, printArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, printArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line numbers - non empty results") {
		clause = ParentTClause(lineOneArg, lineTwoArg);
		expectedRetrievedFromPkb = { pqlParentStarW1A2 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(lineOneArg, lineFourArg);
		expectedRetrievedFromPkb = { pqlParentStarW1R4 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ParentTClause(lineOneArg, lineSixArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(lineTwoArg, lineFourArg);
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, lineFourArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

}
