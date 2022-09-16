#include "catch.hpp"
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ParentClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument("s1", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("s2", ArgumentType::STMT);
		ParentClause parentClause = ParentClause(lhs, rhs);

		// when
		bool actual = parentClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument("s1", ArgumentType::STMT);
	ClauseArgument secondStmtArg = ClauseArgument("s2", ArgumentType::STMT);
	ClauseArgument lineArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);

	SECTION("Equal") {
		RelationshipClause* parentClauseAgain = new ParentClause(firstStmtArg, secondStmtArg);
		testEquals(parentClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		RelationshipClause* parentClauseDiffLhs = new ParentClause(secondStmtArg, secondStmtArg);
		testEquals(parentClauseDiffLhs, false);

		RelationshipClause* parentClauseDiffRhs = new ParentClause(firstStmtArg, firstStmtArg);
		testEquals(parentClauseDiffRhs, false);

		RelationshipClause* parentClauseDiffArgType = new ParentClause(firstStmtArg, lineArg);
		testEquals(parentClauseDiffArgType, false);
	}

	SECTION("Not equal, ParentClause vs ParentTClause") {
		RelationshipClause* parentTClause = new ParentTClause(firstStmtArg, secondStmtArg);
		testEquals(parentTClause, false);
	}
}

TEST_CASE("ParentTClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument("s1", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("s2", ArgumentType::STMT);
		ParentTClause parentTClause = ParentTClause(lhs, rhs);

		// when
		bool actual = parentTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument("s1", ArgumentType::STMT);
	ClauseArgument secondStmtArg = ClauseArgument("s2", ArgumentType::STMT);
	ClauseArgument lineArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);

	SECTION("Equal") {
		RelationshipClause* parentTClauseAgain = new ParentTClause(firstStmtArg, secondStmtArg);
		testEquals(parentTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		RelationshipClause* parentTClauseDiffLhs = new ParentTClause(secondStmtArg, secondStmtArg);
		testEquals(parentTClauseDiffLhs, false);

		RelationshipClause* parentTClauseDiffRhs = new ParentTClause(firstStmtArg, firstStmtArg);
		testEquals(parentTClauseDiffRhs, false);

		RelationshipClause* parentTClauseDiffArgType = new ParentTClause(firstStmtArg, lineArg);
		testEquals(parentTClauseDiffArgType, false);
	}

	SECTION("Not equal, ParentClause vs ParentTClause") {
		RelationshipClause* parentClause = new ParentClause(firstStmtArg, secondStmtArg);
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
	Entity a1 = Entity::createAssignEntity(2);
	Entity i1 = Entity::createIfEntity(3);
	Entity r1 = Entity::createReadEntity(4);
	Entity r2 = Entity::createReadEntity(5);
	Entity p1 = Entity::createPrintEntity(6);

	// Initialise some additional non-statement entities to check they are not returned
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity x = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("x"));
	Entity y = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("y"));
	Entity oneEntity = Entity::createConstantEntity(3, Token::createIntegerToken("1"));

	// Initialise parent relationships
	Relationship parentW1A1 = Relationship::createParentRelationship(w1, a1);
	Relationship parentW1I1 = Relationship::createParentRelationship(w1, i1);
	Relationship parentI1R1 = Relationship::createParentRelationship(i1, r1);
	Relationship parentI1R2 = Relationship::createParentRelationship(i1, r2);

	// Initialise parentStar relationships
	Relationship parentStarW1A1 = Relationship::createParentTRelationship(w1, a1);
	Relationship parentStarW1I1 = Relationship::createParentTRelationship(w1, i1);
	Relationship parentStarI1R1 = Relationship::createParentTRelationship(i1, r1);
	Relationship parentStarI1R2 = Relationship::createParentTRelationship(i1, r2);
	Relationship parentStarW1R1 = Relationship::createParentTRelationship(w1, r1);
	Relationship parentStarW1R2 = Relationship::createParentTRelationship(w1, r2);


	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlW1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA1 = PQLEntity::generateStatement(2);
	PQLEntity pqlI1 = PQLEntity::generateStatement(3);
	PQLEntity pqlR1 = PQLEntity::generateStatement(4);
	PQLEntity pqlR2 = PQLEntity::generateStatement(5);
	PQLEntity pqlP1 = PQLEntity::generateStatement(6);

	PQLRelationship pqlParentW1A1 = PQLRelationship(pqlW1, pqlA1);
	PQLRelationship pqlParentW1I1 = PQLRelationship(pqlW1, pqlI1);
	PQLRelationship pqlParentI1R1 = PQLRelationship(pqlI1, pqlR1);
	PQLRelationship pqlParentI1R2 = PQLRelationship(pqlI1, pqlR2);

	PQLRelationship pqlParentStarW1A1 = PQLRelationship(pqlW1, pqlA1);
	PQLRelationship pqlParentStarW1I1 = PQLRelationship(pqlW1, pqlI1);
	PQLRelationship pqlParentStarI1R1 = PQLRelationship(pqlI1, pqlR1);
	PQLRelationship pqlParentStarI1R2 = PQLRelationship(pqlI1, pqlR2);
	PQLRelationship pqlParentStarW1R1 = PQLRelationship(pqlW1, pqlR1);
	PQLRelationship pqlParentStarW1R2 = PQLRelationship(pqlW1, pqlR2);

	// Clause Arguments
	ClauseArgument firstStmtArg = ClauseArgument("s1", ArgumentType::STMT);
	ClauseArgument secondStmtArg = ClauseArgument("s2", ArgumentType::STMT);
	ClauseArgument ifArg = ClauseArgument("if", ArgumentType::IF);
	ClauseArgument whileArg = ClauseArgument("while", ArgumentType::WHILE);
	ClauseArgument lineOneArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);
	ClauseArgument lineTwoArg = ClauseArgument("2", ArgumentType::LINE_NUMBER);
	ClauseArgument lineFourArg = ClauseArgument("4", ArgumentType::LINE_NUMBER);
	ClauseArgument lineSixArg = ClauseArgument("6", ArgumentType::LINE_NUMBER);
	ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);
	ClauseArgument readArg = ClauseArgument("rr", ArgumentType::READ);
	ClauseArgument printArg = ClauseArgument("pp", ArgumentType::PRINT);
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
	vector<Entity> entities{ w1, a1, i1, r1, r2, p1, main, x, y, oneEntity };
	vector<Relationship> relationships{ parentW1A1, parentW1I1, parentI1R1, parentI1R2,
		parentStarW1A1, parentStarW1I1, parentStarI1R1, parentStarI1R2, parentStarW1R1, parentStarW1R2 };
	pkb -> addEntities(entities);
	pkb -> addRelationships(relationships);

	
	// ------ QPS ------ 
	ParentClause clause = ParentClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlParentW1A1, pqlParentW1I1, pqlParentI1R1, pqlParentI1R2 };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);
	SECTION("Two stmt synoynms - all parent results") {
		testExecute(clause, expectedClauseResult, pkb);
	}
	
	SECTION("Other stmtRef synonyms - non empty results") {
		clause = ParentClause(ifArg, readArg);
		expectedRetrievedFromPkb = { pqlParentI1R1, pqlParentI1R2 };
		expectedClauseResult = RelationshipClauseResult(ifArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentClause(whileArg, ifArg);
		expectedRetrievedFromPkb = { pqlParentW1I1 };
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
		expectedRetrievedFromPkb = { pqlParentW1A1 };
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
	vector<Entity> entities{ w1, a1, i1, r1, r2, p1, main, x, y, oneEntity };
	vector<Relationship> relationships{
		parentW1A1,
		parentW1I1,
		parentI1R1,
		parentI1R2,
		parentStarW1A1,
		parentStarW1I1,
		parentStarI1R1,
		parentStarI1R2,
		parentStarW1R1,
		parentStarW1R2
	};
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	ParentTClause clause = ParentTClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlParentStarW1A1, pqlParentStarW1I1,
		pqlParentStarI1R1, pqlParentStarI1R2, pqlParentStarW1R1, pqlParentStarW1R2
	};
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);
	SECTION("Two stmt synoynms - all parentStar results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = ParentTClause(ifArg, readArg);
		expectedRetrievedFromPkb = { pqlParentStarI1R1, pqlParentStarI1R2 };
		expectedClauseResult = RelationshipClauseResult(ifArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(whileArg, ifArg);
		expectedRetrievedFromPkb = { pqlParentStarW1I1 };
		expectedClauseResult = RelationshipClauseResult(whileArg, ifArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(whileArg, readArg);
		expectedRetrievedFromPkb = { pqlParentStarW1R1, pqlParentStarW1R2 };
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
		expectedRetrievedFromPkb = { pqlParentStarW1A1 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ParentTClause(lineOneArg, lineFourArg);
		expectedRetrievedFromPkb = { pqlParentStarW1R1 };
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