#include "catch.hpp"
#include <qps/query_evaluator/QueryEvaluator.h>
#include <qps/query_evaluator/QueryEvaluator.cpp>

using namespace std;

// ==================== UNIT TESTS ====================

TEST_CASE("QueryEvaluator: test findArgumentIndex") {
	auto testFindArgumentIndex = [](vector<ClauseArgument> argumentsInTable,
			ClauseArgument argToFind, int expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		int actual = evaluator.findArgumentIndex(argumentsInTable, argToFind);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument stmtArg = ClauseArgument("s", ArgumentType::STMT);
	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
	ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument constArg = ClauseArgument("4", ArgumentType::CONSTANT);

	ClauseArgument readArg = ClauseArgument("r", ArgumentType::READ);
	ClauseArgument printArg = ClauseArgument("p", ArgumentType::PRINT);
	ClauseArgument callArg = ClauseArgument("c", ArgumentType::CALL);
	ClauseArgument whileArg = ClauseArgument("w", ArgumentType::WHILE);
	ClauseArgument ifArg = ClauseArgument("i", ArgumentType::IF);
	ClauseArgument stringLitArg = ClauseArgument("x", ArgumentType::STRING_LITERAL);
	ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);

	vector<ClauseArgument> argumentsInTable = {stmtArg, assignArg, procArg, varArg, constArg};
	vector<ClauseArgument> argumentsNotInTable = {readArg, printArg, callArg, whileArg, ifArg, stringLitArg, wildcardArg};

	SECTION("Argument in table") {
		testFindArgumentIndex(argumentsInTable, ClauseArgument("s", ArgumentType::STMT), 0);
		testFindArgumentIndex(argumentsInTable, ClauseArgument("a", ArgumentType::ASSIGN), 1);
		testFindArgumentIndex(argumentsInTable, ClauseArgument("p", ArgumentType::PROCEDURE), 2);
		testFindArgumentIndex(argumentsInTable, ClauseArgument("v", ArgumentType::VARIABLE), 3);
		testFindArgumentIndex(argumentsInTable, ClauseArgument("4", ArgumentType::CONSTANT), 4);
	}

	SECTION("Argument not in table") {
		for (ClauseArgument argument : argumentsNotInTable) {
			testFindArgumentIndex(argumentsInTable, argument, -1);
		}
	}
}

TEST_CASE("QueryEvaluator: test getTable") {
	auto testGetTable = [](RelationshipClauseResult relationshipsResult,
			vector<vector<PQLEntity>> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<vector<PQLEntity>> actual = evaluator.getTable(relationshipsResult);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);

	PQLEntity firstRelationshipFirstEntity = PQLEntity::generateStatement(1);
	PQLEntity firstRelationshipSecondEntity = PQLEntity::generateVariable("x");
	PQLEntity secondRelationshipFirstEntity = PQLEntity::generateStatement(2);
	PQLEntity secondRelationshipSecondEntity = PQLEntity::generateVariable("y");

	PQLRelationship firstRelationship = PQLRelationship(firstRelationshipFirstEntity, firstRelationshipSecondEntity);
	PQLRelationship secondRelationship = PQLRelationship(secondRelationshipFirstEntity, secondRelationshipSecondEntity);
	RelationshipClauseResult relationshipsResult = RelationshipClauseResult(assignArg, varArg,
																			{firstRelationship, secondRelationship});

	vector<vector<PQLEntity>> expectedVector = {
			{firstRelationshipFirstEntity, firstRelationshipSecondEntity},
			{secondRelationshipFirstEntity, secondRelationshipSecondEntity}
	};

	testGetTable(relationshipsResult, expectedVector);
}

TEST_CASE("QueryEvaluator: test getKeyValueTable") {
	auto testGetKeyValueTable = [](
			RelationshipClauseResult relationshipsResult, KeyColumn keyColumn, vector<vector<PQLEntity>> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<vector<PQLEntity>> actual = evaluator.getKeyValueTable(relationshipsResult, keyColumn);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);

	PQLEntity firstRelationshipFirstEntity = PQLEntity::generateStatement(1);
	PQLEntity firstRelationshipSecondEntity = PQLEntity::generateVariable("x");
	PQLEntity secondRelationshipFirstEntity = PQLEntity::generateStatement(2);
	PQLEntity secondRelationshipSecondEntity = PQLEntity::generateVariable("y");

	PQLRelationship firstRelationship = PQLRelationship(firstRelationshipFirstEntity, firstRelationshipSecondEntity);
	PQLRelationship secondRelationship = PQLRelationship(secondRelationshipFirstEntity, secondRelationshipSecondEntity);
	RelationshipClauseResult relationshipsResult = RelationshipClauseResult(assignArg, varArg,
																			{firstRelationship, secondRelationship});

	vector<vector<PQLEntity>> expectedFirstColumnKeyVector = {
			{firstRelationshipFirstEntity, firstRelationshipSecondEntity},
			{secondRelationshipFirstEntity, secondRelationshipSecondEntity}
	};

	vector<vector<PQLEntity>> expectedSecondColumnKeyVector = {
			{firstRelationshipSecondEntity, firstRelationshipFirstEntity},
			{secondRelationshipSecondEntity, secondRelationshipFirstEntity}
	};

	testGetKeyValueTable(relationshipsResult, KeyColumn::FIRST_COLUMN_KEY, expectedFirstColumnKeyVector);
	testGetKeyValueTable(relationshipsResult, KeyColumn::SECOND_COLUMN_KEY, expectedSecondColumnKeyVector);
}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsNoError with single key") {
	auto testConvertToKeyValuePairsNoErrorSingleKey = [](vector<vector<PQLEntity>> table, int key,
			vector<pair<PQLEntity, vector<PQLEntity>>> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<pair<PQLEntity, vector<PQLEntity>>> actual = evaluator.convertToKeyValuePairs(table, key);

		// then
		REQUIRE(actual == expected);
	};

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	PQLEntity firstProcEntity = PQLEntity::generateProcedure("p1");
	PQLEntity secondProcEntity = PQLEntity::generateProcedure("p2");
	PQLEntity thirdProcEntity = PQLEntity::generateProcedure("p3");

	PQLEntity firstConstEntity = PQLEntity::generateConstant(4);
	PQLEntity secondConstEntity = PQLEntity::generateConstant(5);
	PQLEntity thirdConstEntity = PQLEntity::generateConstant(6);

	vector<PQLEntity> firstRow = {firstStmtEntity, firstVarEntity, firstProcEntity, firstConstEntity};
	vector<PQLEntity> secondRow = {secondStmtEntity, secondVarEntity, secondProcEntity, secondConstEntity};
	vector<PQLEntity> thirdRow = {thirdStmtEntity, thirdVarEntity, thirdProcEntity, thirdConstEntity};
	vector<vector<PQLEntity>> table = {firstRow, secondRow, thirdRow};

	int firstColumnKey = 0;
	vector<pair<PQLEntity, vector<PQLEntity>>> firstColumnKeyExpectedPairs = {
			{firstStmtEntity, firstRow},
			{secondStmtEntity, secondRow},
			{thirdStmtEntity, thirdRow}
	};

	int secondColumnKey = 1;
	vector<pair<PQLEntity, vector<PQLEntity>>> secondColumnKeyExpectedPairs = {
			{firstVarEntity, firstRow},
			{secondVarEntity, secondRow},
			{thirdVarEntity, thirdRow}
	};

	int thirdColumnKey = 2;
	vector<pair<PQLEntity, vector<PQLEntity>>> thirdColumnKeyExpectedPairs = {
			{firstProcEntity, firstRow},
			{secondProcEntity, secondRow},
			{thirdProcEntity, thirdRow}
	};

	int fourthColumnKey = 3;
	vector<pair<PQLEntity, vector<PQLEntity>>> fourthColumnKeyExpectedPairs = {
			{firstConstEntity, firstRow},
			{secondConstEntity, secondRow},
			{thirdConstEntity, thirdRow}
	};

	testConvertToKeyValuePairsNoErrorSingleKey(table, firstColumnKey, firstColumnKeyExpectedPairs);
	testConvertToKeyValuePairsNoErrorSingleKey(table, secondColumnKey, secondColumnKeyExpectedPairs);
	testConvertToKeyValuePairsNoErrorSingleKey(table, thirdColumnKey, thirdColumnKeyExpectedPairs);
	testConvertToKeyValuePairsNoErrorSingleKey(table, fourthColumnKey, fourthColumnKeyExpectedPairs);

}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsWithError with single key") {
	auto testConvertToKeyValuePairsWithErrorSingleKey = [](vector<vector<PQLEntity>> table, int key) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when and then
		REQUIRE_THROWS_AS(evaluator.convertToKeyValuePairs(table, key), PQLError);
	};

	SECTION("Invalid key") {
		PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
		PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
		PQLEntity firstProcEntity = PQLEntity::generateProcedure("p1");
		PQLEntity firstConstEntity = PQLEntity::generateConstant(4);

		vector<PQLEntity> firstRow = {firstStmtEntity, firstVarEntity, firstProcEntity, firstConstEntity};
		vector<vector<PQLEntity>> table = {firstRow};

		testConvertToKeyValuePairsWithErrorSingleKey(table, -1);
		testConvertToKeyValuePairsWithErrorSingleKey(table, 4);
	}

	SECTION("Empty table") {
		vector<vector<PQLEntity>> emptyTable = {};
		testConvertToKeyValuePairsWithErrorSingleKey(emptyTable, 0);
	}

}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsNoError with pair key") {

	auto testConvertToKeyValuePairsNoErrorPairKey = [](vector<vector<PQLEntity>> table,
			int firstKey, int secondKey, vector<pair<vector<PQLEntity>, vector<PQLEntity>>> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> actual = evaluator.convertToKeyValuePairs(
				table, firstKey, secondKey);

		// then
		REQUIRE(actual == expected);
	};

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	PQLEntity firstProcEntity = PQLEntity::generateProcedure("p1");
	PQLEntity secondProcEntity = PQLEntity::generateProcedure("p2");
	PQLEntity thirdProcEntity = PQLEntity::generateProcedure("p3");

	PQLEntity firstConstEntity = PQLEntity::generateConstant(4);
	PQLEntity secondConstEntity = PQLEntity::generateConstant(5);
	PQLEntity thirdConstEntity = PQLEntity::generateConstant(6);

	vector<PQLEntity> firstRow = {firstStmtEntity, firstVarEntity, firstProcEntity, firstConstEntity};
	vector<PQLEntity> secondRow = {secondStmtEntity, secondVarEntity, secondProcEntity, secondConstEntity};
	vector<PQLEntity> thirdRow = {thirdStmtEntity, thirdVarEntity, thirdProcEntity, thirdConstEntity};
	vector<vector<PQLEntity>> table = {firstRow, secondRow, thirdRow};

	int adjacentColumnsFirstKey = 0;
	int adjacentColumnsSecondKey = 1;
	vector<pair<vector<PQLEntity>, vector<PQLEntity>>> expectedPairsAdjacentColumnsKey = {
			{{firstStmtEntity, firstVarEntity}, firstRow},
			{{secondStmtEntity, secondVarEntity}, secondRow},
			{{thirdStmtEntity, thirdVarEntity}, thirdRow}
	};

	int splitColumnsFirstKey = 0;
	int splitColumnsSecondKey = 3;
	vector<pair<vector<PQLEntity>, vector<PQLEntity>>> expectedPairsSplitColumnsKey = {
			{{firstStmtEntity, firstConstEntity}, firstRow},
			{{secondStmtEntity, secondConstEntity}, secondRow},
			{{thirdStmtEntity, thirdConstEntity}, thirdRow}
	};

	int reverseOrderColumnsFirstKey = 2;
	int reverseOrderColumnsSecondKey = 0;
	vector<pair<vector<PQLEntity>, vector<PQLEntity>>> expectedPairsReverseOrderColumnsKey = {
			{{firstProcEntity, firstStmtEntity}, firstRow},
			{{secondProcEntity, secondStmtEntity}, secondRow},
			{{thirdProcEntity, thirdStmtEntity}, thirdRow}
	};


	testConvertToKeyValuePairsNoErrorPairKey(table,adjacentColumnsFirstKey,
											 adjacentColumnsSecondKey, expectedPairsAdjacentColumnsKey);
	testConvertToKeyValuePairsNoErrorPairKey(table, splitColumnsFirstKey,
											 splitColumnsSecondKey, expectedPairsSplitColumnsKey);
	testConvertToKeyValuePairsNoErrorPairKey(table, reverseOrderColumnsFirstKey,
											 reverseOrderColumnsSecondKey, expectedPairsReverseOrderColumnsKey);
}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsWithError with pair key") {
	auto testConvertToKeyValuePairsWithErrorPairKey = [](vector<vector<PQLEntity>> table, int firstKey, int secondKey) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when and then
		REQUIRE_THROWS_AS(evaluator.convertToKeyValuePairs(table, firstKey, secondKey), PQLError);
	};

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	PQLEntity firstProcEntity = PQLEntity::generateProcedure("p1");
	PQLEntity secondProcEntity = PQLEntity::generateProcedure("p2");
	PQLEntity thirdProcEntity = PQLEntity::generateProcedure("p3");

	PQLEntity firstConstEntity = PQLEntity::generateConstant(4);
	PQLEntity secondConstEntity = PQLEntity::generateConstant(5);
	PQLEntity thirdConstEntity = PQLEntity::generateConstant(6);

	vector<PQLEntity> firstRow = {firstStmtEntity, firstVarEntity, firstProcEntity, firstConstEntity};
	vector<PQLEntity> secondRow = {secondStmtEntity, secondVarEntity, secondProcEntity, secondConstEntity};
	vector<PQLEntity> thirdRow = {thirdStmtEntity, thirdVarEntity, thirdProcEntity, thirdConstEntity};
	vector<vector<PQLEntity>> table = {firstRow, secondRow, thirdRow};

	SECTION("Invalid key") {
		// Should show first key error message
		testConvertToKeyValuePairsWithErrorPairKey(table, -1, 0);

		// Should show second key error message
		testConvertToKeyValuePairsWithErrorPairKey(table, 0, -1);

		// Should show first key error message (checks first then second)
		testConvertToKeyValuePairsWithErrorPairKey(table, -1, -1);
	}

	SECTION("Duplicate key") {
		testConvertToKeyValuePairsWithErrorPairKey(table, 0, 0);
		testConvertToKeyValuePairsWithErrorPairKey(table, 1, 1);
		testConvertToKeyValuePairsWithErrorPairKey(table, 2, 2);
		testConvertToKeyValuePairsWithErrorPairKey(table, 3, 3);
	}

	SECTION("Empty table") {
		vector<vector<PQLEntity>> emptyTable = {};
		testConvertToKeyValuePairsWithErrorPairKey(emptyTable, 0, 0);
	}
}

TEST_CASE("QueryEvaluator: test singleKeyTableJoin") {}

TEST_CASE("QueryEvaluator: test pairKeyTableJoin") {}

TEST_CASE("QueryEvaluator: test combinedTableJoin") {}

TEST_CASE("QueryEvaluator: test filterEntities") {}

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

// TODO: Waiting for PKB interaction implementation
TEST_CASE("QueryEvaluator: test evaluate") {}
