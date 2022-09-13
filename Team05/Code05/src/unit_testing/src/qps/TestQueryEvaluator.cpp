#include "catch.hpp"
#include <qps/query_evaluator/QueryEvaluator.h>
#include <qps/query_evaluator/QueryEvaluator.cpp>

using namespace std;

// ==================== UNIT TESTS ====================

TEST_CASE("QueryEvaluator: test dereferenceEntitiesResultPointer") {
	auto testDereferenceEntitiesResultPointer = [](
			shared_ptr<EntityClauseResult> entitiesResultPointer, EntityClauseResult expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		EntityClauseResult actual = evaluator.dereferenceEntitiesResultPointer(entitiesResultPointer);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	shared_ptr<EntityClauseResult> testClauseResult = shared_ptr<EntityClauseResult>(new EntityClauseResult(
			varArg, vector<PQLEntity>{firstVarEntity, secondVarEntity}));

	EntityClauseResult expectedEntityClauseResult = EntityClauseResult(
			varArg, vector<PQLEntity>{firstVarEntity, secondVarEntity});

	testDereferenceEntitiesResultPointer(testClauseResult, expectedEntityClauseResult);
}

TEST_CASE("QueryEvaluator: test dereferenceRelationshipsResultPointers") {
	auto testDereferenceRelationshipsResultPointers = [](
			list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers, list<RelationshipClauseResult> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		list<RelationshipClauseResult> actual = evaluator.dereferenceRelationshipsResultPointers(
				relationshipsResultPointers);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument stmtArg = ClauseArgument("s", ArgumentType::LINE_NUMBER);
	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");

	PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
	PQLRelationship secondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);

	shared_ptr<RelationshipClauseResult> testClauseResult = shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(
			stmtArg, varArg, {firstRelationship, secondRelationship}));

	RelationshipClauseResult expectedRelationshipClauseResult = RelationshipClauseResult(
			stmtArg, varArg, {firstRelationship, secondRelationship});

	testDereferenceRelationshipsResultPointers(
			list<shared_ptr<RelationshipClauseResult>>{testClauseResult},
			list<RelationshipClauseResult>{expectedRelationshipClauseResult});
}

TEST_CASE("QueryEvaluator: test findDesiredArgument") {
	auto testFindDesiredArgument = [](ClauseArgument desiredArg,
			RelationshipClauseResult relationshipResultToCheck,RelationshipArgument expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		RelationshipArgument actual = evaluator.findDesiredArgument(desiredArg, relationshipResultToCheck);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstVarArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument secondVarArg = ClauseArgument("v1", ArgumentType::VARIABLE);
	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");

	PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
	PQLRelationship secondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);

	RelationshipClauseResult firstRelationshipClauseResult = RelationshipClauseResult(
			assignArg, firstVarArg, {firstRelationship, secondRelationship});
	RelationshipClauseResult secondRelationshipClauseResult = RelationshipClauseResult(
			assignArg, secondVarArg, {firstRelationship, secondRelationship});

	SECTION("First arg matches") {
		ClauseArgument desiredArg = ClauseArgument("a", ArgumentType::ASSIGN);
		testFindDesiredArgument(desiredArg, firstRelationshipClauseResult, RelationshipArgument::ARG1);
		testFindDesiredArgument(desiredArg, secondRelationshipClauseResult, RelationshipArgument::ARG1);
	}

	SECTION("Second arg matches") {
		ClauseArgument desiredArg = ClauseArgument("v", ArgumentType::VARIABLE);
		testFindDesiredArgument(desiredArg, firstRelationshipClauseResult, RelationshipArgument::ARG2);

		desiredArg = ClauseArgument("v1", ArgumentType::VARIABLE);
		testFindDesiredArgument(desiredArg, secondRelationshipClauseResult, RelationshipArgument::ARG2);
	}

	SECTION("Neither arg matches") {
		ClauseArgument desiredArg = ClauseArgument("v", ArgumentType::VARIABLE);
		testFindDesiredArgument(desiredArg, secondRelationshipClauseResult, RelationshipArgument::NONE);

		desiredArg = ClauseArgument("p", ArgumentType::PROCEDURE);
		testFindDesiredArgument(desiredArg, firstRelationshipClauseResult, RelationshipArgument::NONE);
		testFindDesiredArgument(desiredArg, secondRelationshipClauseResult, RelationshipArgument::NONE);
	}
}

TEST_CASE("QueryEvaluator: test extractEntitySetNoError") {
	auto testExtractEntitySetNoError = [](
			RelationshipArgument argToExtract, vector<PQLRelationship> relationships, set<PQLEntity> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		set<PQLEntity> actual = evaluator.extractEntitySet(argToExtract, relationships);

		// then
		REQUIRE(actual == expected);
	};

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
	PQLRelationship secondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
	PQLRelationship thirdRelationship = PQLRelationship(thirdStmtEntity, thirdVarEntity);

	vector<PQLRelationship> relationships = {firstRelationship, secondRelationship, thirdRelationship};

	set<PQLEntity> stmtSet = {firstStmtEntity, secondStmtEntity, thirdStmtEntity};
	set<PQLEntity> varSet = {firstVarEntity, secondVarEntity, thirdVarEntity};

	testExtractEntitySetNoError(RelationshipArgument::ARG1, relationships, stmtSet);
	testExtractEntitySetNoError(RelationshipArgument::ARG2, relationships, varSet);

}

TEST_CASE("QueryEvaluator: test extractEntitySetWithError") {
	auto testExtractEntitySetWithError = [](
			RelationshipArgument argToExtract, vector<PQLRelationship> relationships) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when, then
		REQUIRE_THROWS_AS(evaluator.extractEntitySet(argToExtract, relationships), PQLError);
	};

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");

	PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);

	testExtractEntitySetWithError(RelationshipArgument::NONE, {firstRelationship});
}

TEST_CASE("QueryEvaluator: test intersectSets") {
	auto testIntersectSets = [](set<PQLEntity> firstSet, set<PQLEntity> secondSet,
			set<PQLEntity> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		set<PQLEntity> actual = evaluator.intersectSets(firstSet, secondSet);

		// then
		REQUIRE(actual == expected);
	};

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	set<PQLEntity> firstSet = {firstVarEntity, secondVarEntity};
	set<PQLEntity> secondSet = {firstVarEntity, thirdVarEntity};
	set<PQLEntity> thirdSet = {secondVarEntity, thirdVarEntity};
	set<PQLEntity> emptySet = {};
	set<PQLEntity> anotherEmptySet = {};

	SECTION("Sets with common elements") {
		testIntersectSets(firstSet, secondSet, {firstVarEntity});
		testIntersectSets(firstSet, thirdSet, {secondVarEntity});
		testIntersectSets(secondSet, thirdSet, {thirdVarEntity});
	}

	SECTION("Empty sets") {
		testIntersectSets(emptySet, anotherEmptySet, {});
	}

	SECTION("One empty set") {
		testIntersectSets(firstSet, emptySet, {});
		testIntersectSets(anotherEmptySet, secondSet, {});
	}

	SECTION("No common elements") {
		PQLEntity fourthVarEntity = PQLEntity::generateVariable("a");
		set<PQLEntity> fourthSet = {fourthVarEntity};

		testIntersectSets(firstSet, fourthSet, {});
		testIntersectSets(fourthSet, secondSet, {});

	}
}

TEST_CASE("QueryEvaluator: test combine") {
	auto testCombine = [](shared_ptr<EntityClauseResult> entitiesResultPointer,
		list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers, set<string> expected) {
    // given
    QueryEvaluator evaluator = QueryEvaluator();

    // when
    set<string> actual = evaluator.combine(entitiesResultPointer, relationshipsResultPointers);

    // then
    REQUIRE(actual == expected);
    };


    SECTION("SelectClause only") {
        // x = x + 1; variable v; Select v
        ClauseArgument variableArg = ClauseArgument("v", ArgumentType::VARIABLE);
        PQLEntity varEntity = PQLEntity::generateVariable("x");
        shared_ptr<EntityClauseResult> entityClauseResultWithVariable =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						variableArg,vector<PQLEntity>{varEntity}));

        set<string> expectedSet = set<string>{"x"};

        testCombine(entityClauseResultWithVariable,
					list<shared_ptr<RelationshipClauseResult>>{},
					expectedSet);

        // x = x + 1; constant c; Select c
        ClauseArgument constantArg = ClauseArgument("c", ArgumentType::CONSTANT);
        PQLEntity constEntity = PQLEntity::generateConstant(1);
        shared_ptr<EntityClauseResult> entityClauseResultWithConstant =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						constantArg,vector<PQLEntity>{constEntity}));

        expectedSet = set<string>{"1"};

        testCombine(entityClauseResultWithConstant,
					list<shared_ptr<RelationshipClauseResult>>{},
					expectedSet);

        // x = x + 1; procedure p; Select p
        ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
        shared_ptr<EntityClauseResult> entityClauseResultWithProcedure =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						procArg,vector<PQLEntity>{}));

        expectedSet = set<string>{};

        testCombine(entityClauseResultWithProcedure,
					list<shared_ptr<RelationshipClauseResult>>{},
					expectedSet);
    }

    SECTION("SelectClause with ModifiesSClause") {

        ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
        ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);

        PQLEntity relationship1Entity1 = PQLEntity::generateStatement(1);
        PQLEntity relationship1Entity2 = PQLEntity::generateVariable("x");
        PQLEntity relationship2Entity1 = PQLEntity::generateStatement(2);
        PQLEntity relationship2Entity2 = PQLEntity::generateVariable("y");

        PQLRelationship relationship1 = PQLRelationship(relationship1Entity1, relationship1Entity2);
        PQLRelationship relationship2 = PQLRelationship(relationship2Entity1, relationship2Entity2);

        vector<PQLEntity> stmtEntities = vector<PQLEntity>{relationship1Entity1, relationship2Entity1};
        vector<PQLEntity> varEntities = vector<PQLEntity>{relationship1Entity2, relationship2Entity2};

        shared_ptr<EntityClauseResult> assignEntityClauseResult = shared_ptr<EntityClauseResult>(
				new EntityClauseResult(assignArg, stmtEntities));

        shared_ptr<RelationshipClauseResult> assignAndVarRelationshipClauseResult(new RelationshipClauseResult(
                assignArg,
                varArg,
                vector<PQLRelationship>{relationship1, relationship2}));

        set<string> expectedSet = set<string>{"1", "2"};

        // x = x + 1; y = y + 1; assign a; variable v; Select a such that Modifies(a,v)
        testCombine(assignEntityClauseResult,
					list<shared_ptr<RelationshipClauseResult>>{assignAndVarRelationshipClauseResult},
					expectedSet);

        // x = x + 1; y = y + 1; assign a; variable v; Select v such that Modifies(a,v)
        shared_ptr<EntityClauseResult> varEntityClauseResult = shared_ptr<EntityClauseResult>(
				new EntityClauseResult(varArg, varEntities));

        expectedSet = set<string>{"x", "y"};

        testCombine(varEntityClauseResult,
					list<shared_ptr<RelationshipClauseResult>>{assignAndVarRelationshipClauseResult},
					expectedSet);
    }

}

// ==================== INTEGRATION TESTS ====================

TEST_CASE("QueryEvaluator: test evaluate") {}
