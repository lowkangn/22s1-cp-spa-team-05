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

// =============== INTEGRATION TESTS ====================
#include <iostream>
TEST_CASE("ParentClause: test execute") {
	auto testExecute = [](ParentClause parentClause, 
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = static_pointer_cast<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = parentClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;
			
			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	
	//initialise entities
	Entity w1 = Entity::createWhileEntity(1);
	Entity a1 = Entity::createAssignEntity(2);
	Entity i1 = Entity::createIfEntity(3);
	Entity r1 = Entity::createReadEntity(4);
	Entity r2 = Entity::createReadEntity(5);
	Entity p1 = Entity::createPrintEntity(6);

	//initialise parent relationships
	Relationship parentW1A1 = Relationship(w1, a1, RelationshipType::PARENT);
	Relationship parentW1I1 = Relationship(w1, i1, RelationshipType::PARENT);
	Relationship parentI1R1 = Relationship(i1, r1, RelationshipType::PARENT);
	Relationship parentI1R2 = Relationship(i1, r2, RelationshipType::PARENT);

	//initialise parentStar relationships
	Relationship parentStarW1A1 = Relationship(w1, a1, RelationshipType::PARENTT);
	Relationship parentStarW1I1 = Relationship(w1, i1, RelationshipType::PARENTT);
	Relationship parentStarI1R1 = Relationship(i1, r1, RelationshipType::PARENTT);
	Relationship parentStarI1R2 = Relationship(i1, r2, RelationshipType::PARENTT);
	Relationship parentStarW1R1 = Relationship(w1, r1, RelationshipType::PARENTT);
	Relationship parentStarW1R2 = Relationship(w1, r2, RelationshipType::PARENTT);

	//add to pkb
	vector<Entity> entities{ w1, a1, i1, r1, r2, p1 };
	vector<Relationship> relationships{ parentW1A1, parentW1I1, parentI1R1, parentI1R2,
		parentStarW1A1, parentStarW1I1, parentStarI1R1, parentStarI1R2, parentStarW1R1, parentStarW1R2 };
	pkb -> addEntities(entities);
	pkb -> addRelationships(relationships);

	
	// ------ QPS ------ 
	// Initialise corresponding PQLEntities and PQLRleationships
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
	ClauseArgument lineSixArg = ClauseArgument("6", ArgumentType::LINE_NUMBER);
	ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);
	ClauseArgument readArg = ClauseArgument("rr", ArgumentType::READ);
	ClauseArgument printArg = ClauseArgument("pp", ArgumentType::PRINT);

	// Clauses with expected results
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

		clause = ParentClause(lineTwoArg, lineSixArg);
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

}
