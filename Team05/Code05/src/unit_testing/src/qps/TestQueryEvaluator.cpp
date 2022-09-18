#include "catch.hpp"
#include <qps/query_evaluator/QueryEvaluator.h>

#include <memory>

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

	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constArg = ClauseArgument::createConstantArg("4");

	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pr");
	ClauseArgument callArg = ClauseArgument::createCallArg("c");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	vector<ClauseArgument> argumentsInTable = {stmtArg, assignArg, procArg, varArg, constArg};
	vector<ClauseArgument> argumentsNotInTable = {readArg, printArg, callArg, whileArg, ifArg, stringLitArg, wildcardArg};

	SECTION("Argument in table") {
		testFindArgumentIndex(argumentsInTable, ClauseArgument::createStmtArg("s"), 0);
		testFindArgumentIndex(argumentsInTable, ClauseArgument::createAssignArg("a"), 1);
		testFindArgumentIndex(argumentsInTable, ClauseArgument::createProcedureArg("p"), 2);
		testFindArgumentIndex(argumentsInTable, ClauseArgument::createVariableArg("v"), 3);
		testFindArgumentIndex(argumentsInTable, ClauseArgument::createConstantArg("4"), 4);
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

	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");

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

	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");

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
	auto testConvertToKeyValuePairsSingleKeyNoError = [](vector<vector<PQLEntity>> table, int key,
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

	testConvertToKeyValuePairsSingleKeyNoError(table, firstColumnKey, firstColumnKeyExpectedPairs);
	testConvertToKeyValuePairsSingleKeyNoError(table, secondColumnKey, secondColumnKeyExpectedPairs);
	testConvertToKeyValuePairsSingleKeyNoError(table, thirdColumnKey, thirdColumnKeyExpectedPairs);
	testConvertToKeyValuePairsSingleKeyNoError(table, fourthColumnKey, fourthColumnKeyExpectedPairs);

}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsWithError with single key") {
	auto testConvertToKeyValuePairsSingleKeyWithError = [](vector<vector<PQLEntity>> table, int key) {
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

		testConvertToKeyValuePairsSingleKeyWithError(table, -1);
		testConvertToKeyValuePairsSingleKeyWithError(table, 4);
	}

	SECTION("Empty table") {
		vector<vector<PQLEntity>> emptyTable = {};
		testConvertToKeyValuePairsSingleKeyWithError(emptyTable, 0);
	}

}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsNoError with pair key") {

	auto testConvertToKeyValuePairsPairKeyNoError = [](vector<vector<PQLEntity>> table,
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


	testConvertToKeyValuePairsPairKeyNoError(table,adjacentColumnsFirstKey,
											 adjacentColumnsSecondKey, expectedPairsAdjacentColumnsKey);
	testConvertToKeyValuePairsPairKeyNoError(table, splitColumnsFirstKey,
											 splitColumnsSecondKey, expectedPairsSplitColumnsKey);
	testConvertToKeyValuePairsPairKeyNoError(table, reverseOrderColumnsFirstKey,
											 reverseOrderColumnsSecondKey, expectedPairsReverseOrderColumnsKey);
}

TEST_CASE("QueryEvaluator: test convertToKeyValuePairsWithError with pair key") {
	auto testConvertToKeyValuePairsPairKeyWithError = [](vector<vector<PQLEntity>> table, int firstKey, int secondKey) {
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
		testConvertToKeyValuePairsPairKeyWithError(table, -1, 0);

		// Should show second key error message
		testConvertToKeyValuePairsPairKeyWithError(table, 0, -1);

		// Should show first key error message (checks first then second)
		testConvertToKeyValuePairsPairKeyWithError(table, -1, -1);
	}

	SECTION("Duplicate key") {
		testConvertToKeyValuePairsPairKeyWithError(table, 0, 0);
		testConvertToKeyValuePairsPairKeyWithError(table, 1, 1);
		testConvertToKeyValuePairsPairKeyWithError(table, 2, 2);
		testConvertToKeyValuePairsPairKeyWithError(table, 3, 3);
	}

	SECTION("Empty table") {
		vector<vector<PQLEntity>> emptyTable = {};
		testConvertToKeyValuePairsPairKeyWithError(emptyTable, 0, 0);
	}
}

TEST_CASE("QueryEvaluator: test singleKeyTableJoinNoError") {
	auto testSingleKeyTableJoinNoError = [](vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMerge, vector<vector<PQLEntity>> expected) {

		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<vector<PQLEntity>> actual = evaluator.singleKeyTableJoin(combinedTableKeyValuePairs, tableToMerge);

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

	SECTION("Tables have matching keys") {
		vector<PQLEntity> firstTableFirstRow = {firstStmtEntity, firstVarEntity};
		vector<PQLEntity> firstTableSecondRow = {secondStmtEntity, secondVarEntity};
		vector<PQLEntity> firstTableThirdRow = {thirdStmtEntity, thirdVarEntity};

		vector<vector<PQLEntity>> firstTable = {firstTableFirstRow, firstTableSecondRow, firstTableThirdRow};

		vector<pair<PQLEntity, vector<PQLEntity>>> firstTableKeyValuePairs = {
				{firstVarEntity,  firstTableFirstRow},
				{secondVarEntity, firstTableSecondRow},
				{thirdVarEntity,  firstTableThirdRow}
		};

		vector<PQLEntity> secondTableFirstRow = {firstVarEntity, firstProcEntity};
		vector<PQLEntity> secondTableSecondRow = {firstVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableThirdRow = {secondVarEntity, firstProcEntity};
		vector<PQLEntity> secondTableFourthRow = {secondVarEntity, secondProcEntity};

		vector<vector<PQLEntity>> secondTable = {secondTableFirstRow, secondTableSecondRow, secondTableThirdRow,
												 secondTableFourthRow};

		vector<vector<PQLEntity>> expected = {
				{firstStmtEntity,  firstVarEntity,  firstProcEntity},
				{firstStmtEntity,  firstVarEntity,  secondProcEntity},
				{secondStmtEntity, secondVarEntity, firstProcEntity},
				{secondStmtEntity, secondVarEntity, secondProcEntity}
		};

		testSingleKeyTableJoinNoError(firstTableKeyValuePairs, secondTable, expected);
	}

	SECTION("Tables have no matching keys") {
		vector<PQLEntity> firstTableFirstRow = {firstStmtEntity, firstVarEntity};
		vector<vector<PQLEntity>> firstTable = {firstTableFirstRow};
		vector<pair<PQLEntity, vector<PQLEntity>>> firstTableKeyValuePairs = {
				{firstVarEntity,  firstTableFirstRow}
		};

		vector<PQLEntity> secondTableFirstRow = {secondVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableSecondRow = {secondVarEntity, thirdProcEntity};
		vector<PQLEntity> secondTableThirdRow = {thirdVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableFourthRow = {thirdVarEntity, thirdProcEntity};
		vector<vector<PQLEntity>> secondTable = {secondTableFirstRow, secondTableSecondRow, secondTableThirdRow,
												 secondTableFourthRow};

		vector<vector<PQLEntity>> expected = {};

		testSingleKeyTableJoinNoError(firstTableKeyValuePairs, secondTable, expected);
	}
}

TEST_CASE("QueryEvaluator: test singleKeyTableJoinWithError") {
	auto testSingleKeyTableJoinWithError = [](
			vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMerge) {

		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when and then
		REQUIRE_THROWS_AS(evaluator.singleKeyTableJoin(combinedTableKeyValuePairs, tableToMerge), PQLError);

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

	SECTION("combinedTable is empty") {
		vector<pair<PQLEntity, vector<PQLEntity>>> emptyFirstTableKeyValuePairs = {};

		vector<PQLEntity> secondTableFirstRow = {secondVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableSecondRow = {secondVarEntity, thirdProcEntity};
		vector<PQLEntity> secondTableThirdRow = {thirdVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableFourthRow = {thirdVarEntity, thirdProcEntity};
		vector<vector<PQLEntity>> secondTable = {secondTableFirstRow, secondTableSecondRow, secondTableThirdRow,
												 secondTableFourthRow};

		testSingleKeyTableJoinWithError(emptyFirstTableKeyValuePairs, secondTable);
	}

	SECTION("tableToMerge is empty") {
		vector<PQLEntity> firstTableFirstRow = {firstStmtEntity, firstVarEntity};
		vector<vector<PQLEntity>> firstTable = {firstTableFirstRow};
		vector<pair<PQLEntity, vector<PQLEntity>>> firstTableKeyValuePairs = {{firstVarEntity,  firstTableFirstRow}};

		vector<vector<PQLEntity>> emptySecondTable = {};

		testSingleKeyTableJoinWithError(firstTableKeyValuePairs, emptySecondTable);
	}
}

TEST_CASE("QueryEvaluator: test pairKeyTableJoinNoError") {
	auto testPairKeyTableJoinNoError = [](vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs,
											vector<vector<PQLEntity>> tableToMerge, vector<vector<PQLEntity>> expected) {

		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<vector<PQLEntity>> actual = evaluator.pairKeyTableJoin(combinedTableKeyValuePairs, tableToMerge);

		// then
		REQUIRE(actual == expected);

	};

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);
	PQLEntity fourthStmtEntity = PQLEntity::generateStatement(4);

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");
	PQLEntity fourthVarEntity = PQLEntity::generateVariable("zz");

	PQLEntity firstProcEntity = PQLEntity::generateProcedure("p1");
	PQLEntity secondProcEntity = PQLEntity::generateProcedure("p2");
	PQLEntity thirdProcEntity = PQLEntity::generateProcedure("p3");

	SECTION("Tables have matching keys") {
		vector<PQLEntity> firstTableFirstRow = {firstStmtEntity, firstVarEntity, firstProcEntity};
		vector<PQLEntity> firstTableSecondRow = {secondStmtEntity, secondVarEntity, secondProcEntity};
		vector<PQLEntity> firstTableThirdRow = {thirdStmtEntity, thirdVarEntity, thirdProcEntity};

		vector<vector<PQLEntity>> firstTable = {firstTableFirstRow, firstTableSecondRow, firstTableThirdRow};

		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> firstTableKeyValuePairs = {
				{{firstStmtEntity, firstVarEntity}, firstTableFirstRow},
				{{secondStmtEntity, secondVarEntity}, firstTableSecondRow},
				{{thirdStmtEntity, thirdVarEntity}, firstTableThirdRow}
		};

		vector<PQLEntity> secondTableFirstRow = {firstStmtEntity, firstVarEntity};
		vector<PQLEntity> secondTableSecondRow = {secondStmtEntity, secondVarEntity};
		vector<PQLEntity> secondTableThirdRow = {thirdStmtEntity, thirdVarEntity};
		vector<PQLEntity> secondTableFourthRow = {fourthStmtEntity, fourthVarEntity};

		vector<vector<PQLEntity>> secondTable = {secondTableFirstRow, secondTableSecondRow, secondTableThirdRow,
												 secondTableFourthRow};

		vector<vector<PQLEntity>> expected = {
				{firstStmtEntity,  firstVarEntity,  firstProcEntity},
				{secondStmtEntity, secondVarEntity, secondProcEntity},
				{thirdStmtEntity, thirdVarEntity, thirdProcEntity}
		};

		testPairKeyTableJoinNoError(firstTableKeyValuePairs, secondTable, expected);
	}

	SECTION("Tables have no matching keys") {
		vector<PQLEntity> firstTableFirstRow = {firstStmtEntity, firstVarEntity, firstProcEntity};
		vector<PQLEntity> firstTableSecondRow = {secondStmtEntity, secondVarEntity, secondProcEntity};

		vector<vector<PQLEntity>> firstTable = {firstTableFirstRow, firstTableSecondRow};

		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> firstTableKeyValuePairs = {
				{{firstStmtEntity, firstVarEntity}, firstTableFirstRow},
				{{secondStmtEntity, secondVarEntity}, firstTableSecondRow}
		};

		vector<PQLEntity> secondTableFirstRow = {thirdStmtEntity, thirdVarEntity};
		vector<PQLEntity> secondTableSecondRow = {fourthStmtEntity, fourthVarEntity};

		vector<vector<PQLEntity>> secondTable = {secondTableFirstRow, secondTableSecondRow};

		vector<vector<PQLEntity>> expected = {};

		testPairKeyTableJoinNoError(firstTableKeyValuePairs, secondTable, expected);
	}
}

TEST_CASE("QueryEvaluator: test pairKeyTableJoinWithError") {
	auto testPairKeyTableJoinWithError = [](
			vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMerge) {

		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when and then
		REQUIRE_THROWS_AS(evaluator.pairKeyTableJoin(combinedTableKeyValuePairs, tableToMerge), PQLError);

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

	SECTION("combinedTable is empty") {
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> emptyFirstTableKeyValuePairs = {};

		vector<PQLEntity> secondTableFirstRow = {secondVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableSecondRow = {secondVarEntity, thirdProcEntity};
		vector<PQLEntity> secondTableThirdRow = {thirdVarEntity, secondProcEntity};
		vector<PQLEntity> secondTableFourthRow = {thirdVarEntity, thirdProcEntity};
		vector<vector<PQLEntity>> secondTable = {secondTableFirstRow, secondTableSecondRow, secondTableThirdRow,
												 secondTableFourthRow};

		testPairKeyTableJoinWithError(emptyFirstTableKeyValuePairs, secondTable);
	}

	SECTION("tableToMerge is empty") {
		vector<PQLEntity> firstTableFirstRow = {firstStmtEntity, firstVarEntity, firstProcEntity};
		vector<vector<PQLEntity>> firstTable = {firstTableFirstRow};
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> firstTableKeyValuePairs = {
				{{firstStmtEntity, firstVarEntity}, firstTableFirstRow}
		};

		vector<vector<PQLEntity>> emptySecondTable = {};

		testPairKeyTableJoinWithError(firstTableKeyValuePairs, emptySecondTable);
	}
}

TEST_CASE("QueryEvaluator: test combinedTableJoin") {
	auto testCombinedTableJoin = [](
			shared_ptr<vector<vector<PQLEntity>>> combinedTable,
			shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable,
			list<RelationshipClauseResult> relationshipsResults,
			vector<vector<PQLEntity>> expectedCombinedTable,
			vector<ClauseArgument> expectedArguments,
			bool expectedBool) {

		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		bool actual = evaluator.combinedTableJoin(combinedTable, argumentsInCombinedTable, relationshipsResults);

		// then
		REQUIRE(actual == expectedBool);
		REQUIRE(*combinedTable == expectedCombinedTable);
		REQUIRE(*argumentsInCombinedTable == expectedArguments);

	};

	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument constArg = ClauseArgument::createConstantArg("4");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");

	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	PQLEntity firstProcEntity = PQLEntity::generateProcedure("testing");
	PQLEntity secondProcEntity = PQLEntity::generateProcedure("so");
	PQLEntity thirdProcEntity = PQLEntity::generateProcedure("cumbersome");

	PQLEntity firstConstEntity = PQLEntity::generateConstant(4);
	PQLEntity secondConstEntity = PQLEntity::generateConstant(5);
	PQLEntity thirdConstEntity = PQLEntity::generateConstant(6);

	PQLEntity assignEntity = PQLEntity::generateStatement(7);
	PQLEntity readEntity = PQLEntity::generateStatement(8);

	SECTION("Empty table, no tables to merge") {
		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();

		vector<vector<PQLEntity>> expectedCombinedTable = {};

		vector<ClauseArgument> expectedArguments = {};

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {},
							  expectedCombinedTable,
							  expectedArguments,
							  true);
	}

	SECTION("All tables joined") {
		// Create RelationshipClauseResult and set up base combinedTable
		PQLRelationship firstResultFirstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		PQLRelationship firstResultSecondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstResultFirstRelationship, firstResultSecondRelationship});

		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		*combinedTable = {
				{firstStmtEntity, firstVarEntity},
				{secondStmtEntity, secondVarEntity}
		};

		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();
		*argumentsInCombinedTable = {stmtArg, varArg};

		// Create other RelationshipClauseResults
		PQLRelationship secondResultFirstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		PQLRelationship secondResultSecondRelationship = PQLRelationship(secondProcEntity, secondConstEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {secondResultFirstRelationship, secondResultSecondRelationship});

		PQLRelationship thirdResultFirstRelationship = PQLRelationship(firstProcEntity, firstVarEntity);
		PQLRelationship thirdResultSecondRelationship = PQLRelationship(secondProcEntity, secondVarEntity);
		RelationshipClauseResult thirdRelationshipsResult = RelationshipClauseResult(
				procArg, varArg, {thirdResultFirstRelationship, thirdResultSecondRelationship});

		vector<vector<PQLEntity>> expectedCombinedTable = {
				{firstStmtEntity, firstVarEntity, firstProcEntity, firstConstEntity},
				{secondStmtEntity, secondVarEntity, secondProcEntity, secondConstEntity}
		};

		vector<ClauseArgument> expectedArguments = {stmtArg, varArg, procArg, constArg};

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {secondRelationshipsResult, thirdRelationshipsResult},
							  expectedCombinedTable,
							  expectedArguments,
							  true);
	}

	SECTION("Empty table resulting from joining") {
		// Create RelationshipClauseResult and set up base combinedTable
		PQLRelationship firstResultFirstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		PQLRelationship firstResultSecondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstResultFirstRelationship, firstResultSecondRelationship});

		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		*combinedTable = {
				{firstStmtEntity, firstVarEntity},
				{secondStmtEntity, secondVarEntity}
		};

		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();
		*argumentsInCombinedTable = {stmtArg, varArg};

		// Create other RelationshipClauseResults
		PQLRelationship secondResultFirstRelationship = PQLRelationship(thirdStmtEntity, firstConstEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				stmtArg, constArg, {secondResultFirstRelationship});

		vector<vector<PQLEntity>> expectedCombinedTable = *combinedTable;
		vector<ClauseArgument> expectedArguments = *argumentsInCombinedTable;

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {secondRelationshipsResult},
							  expectedCombinedTable,
							  expectedArguments,
							  false);
	}

	SECTION("One table not joined") {
		// Create RelationshipClauseResult and set up base combinedTable
		PQLRelationship firstResultFirstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		PQLRelationship firstResultSecondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstResultFirstRelationship, firstResultSecondRelationship});

		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		*combinedTable = {
				{firstStmtEntity, firstVarEntity},
				{secondStmtEntity, secondVarEntity}
		};

		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();
		*argumentsInCombinedTable = {stmtArg, varArg};

		// Create other RelationshipClauseResults
		PQLRelationship secondResultFirstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {secondResultFirstRelationship});

		vector<vector<PQLEntity>> expectedCombinedTable = *combinedTable;
		vector<ClauseArgument> expectedArguments = *argumentsInCombinedTable;

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {secondRelationshipsResult},
							  expectedCombinedTable,
							  expectedArguments,
							  true);
	}

	SECTION("Two tables not joined, those two cannot join") {
		// Create RelationshipClauseResult and set up base combinedTable
		PQLRelationship firstResultFirstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		PQLRelationship firstResultSecondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstResultFirstRelationship, firstResultSecondRelationship});

		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		*combinedTable = {
				{firstStmtEntity, firstVarEntity},
				{secondStmtEntity, secondVarEntity}
		};

		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();
		*argumentsInCombinedTable = {stmtArg, varArg};

		// Create other RelationshipClauseResults
		PQLRelationship secondResultFirstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {secondResultFirstRelationship});

		PQLRelationship thirdResultFirstRelationship = PQLRelationship(assignEntity, readEntity);
		RelationshipClauseResult thirdRelationshipsResult = RelationshipClauseResult(
				assignArg, readArg, {thirdResultFirstRelationship});

		vector<vector<PQLEntity>> expectedCombinedTable = *combinedTable;
		vector<ClauseArgument> expectedArguments = *argumentsInCombinedTable;

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {secondRelationshipsResult, thirdRelationshipsResult},
							  expectedCombinedTable,
							  expectedArguments,
							  true);
	}

	SECTION("Two tables not joined, those two can join and return empty table") {
		// Create RelationshipClauseResult and set up base combinedTable
		PQLRelationship firstResultFirstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		PQLRelationship firstResultSecondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstResultFirstRelationship, firstResultSecondRelationship});

		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		*combinedTable = {
				{firstStmtEntity, firstVarEntity},
				{secondStmtEntity, secondVarEntity}
		};

		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();
		*argumentsInCombinedTable = {stmtArg, varArg};

		// Create other RelationshipClauseResults
		PQLRelationship secondResultFirstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {secondResultFirstRelationship});

		PQLRelationship thirdResultFirstRelationship = PQLRelationship(secondProcEntity, readEntity);
		RelationshipClauseResult thirdRelationshipsResult = RelationshipClauseResult(
				procArg, readArg, {thirdResultFirstRelationship});

		vector<vector<PQLEntity>> expectedCombinedTable = *combinedTable;
		vector<ClauseArgument> expectedArguments = *argumentsInCombinedTable;

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {secondRelationshipsResult, thirdRelationshipsResult},
							  expectedCombinedTable,
							  expectedArguments,
							  false);
	}

	SECTION("Two tables not joined, those two can join and return non-empty table") {
		// Create RelationshipClauseResult and set up base combinedTable
		PQLRelationship firstResultFirstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		PQLRelationship firstResultSecondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstResultFirstRelationship, firstResultSecondRelationship});

		shared_ptr<vector<vector<PQLEntity>>> combinedTable = make_shared<vector<vector<PQLEntity>>>();
		*combinedTable = {
				{firstStmtEntity, firstVarEntity},
				{secondStmtEntity, secondVarEntity}
		};

		shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable = make_shared<vector<ClauseArgument>>();
		*argumentsInCombinedTable = {stmtArg, varArg};

		// Create other RelationshipClauseResults
		PQLRelationship secondResultFirstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {secondResultFirstRelationship});

		PQLRelationship thirdResultFirstRelationship = PQLRelationship(firstProcEntity, readEntity);
		RelationshipClauseResult thirdRelationshipsResult = RelationshipClauseResult(
				procArg, readArg, {thirdResultFirstRelationship});

		vector<vector<PQLEntity>> expectedCombinedTable = *combinedTable;
		vector<ClauseArgument> expectedArguments = *argumentsInCombinedTable;

		testCombinedTableJoin(combinedTable,
							  argumentsInCombinedTable,
							  {secondRelationshipsResult, thirdRelationshipsResult},
							  expectedCombinedTable,
							  expectedArguments,
							  true);
	}
}

TEST_CASE("QueryEvaluator: test filterEntities") {
	auto testFilterEntities = [](EntityClauseResult entitiesResult,
			list<RelationshipClauseResult> relationshipsResults, vector<PQLEntity> expected) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();

		// when
		vector<PQLEntity> actual = evaluator.filterEntities(entitiesResult, relationshipsResults);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constArg = ClauseArgument::createConstantArg("4");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");


	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);
	PQLEntity fourthStmtEntity = PQLEntity::generateStatement(4);

	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");

	PQLEntity firstProcEntity = PQLEntity::generateProcedure("testing");
	PQLEntity secondProcEntity = PQLEntity::generateProcedure("so");
	PQLEntity thirdProcEntity = PQLEntity::generateProcedure("cumbersome");

	PQLEntity firstConstEntity = PQLEntity::generateConstant(4);
	PQLEntity secondConstEntity = PQLEntity::generateConstant(5);
	PQLEntity thirdConstEntity = PQLEntity::generateConstant(6);

	PQLEntity assignEntity = PQLEntity::generateStatement(7);
	PQLEntity readEntity = PQLEntity::generateStatement(8);

	vector<PQLEntity> stmtEntities = {firstStmtEntity, secondStmtEntity, thirdStmtEntity};
	vector<PQLEntity> varEntities = {firstVarEntity, secondVarEntity, thirdVarEntity};

	EntityClauseResult stmtEntityClauseResult = EntityClauseResult(stmtArg, stmtEntities);
	EntityClauseResult varEntityClauseResult = EntityClauseResult(varArg, varEntities);

	SECTION("Empty list") {
		testFilterEntities(stmtEntityClauseResult, {}, stmtEntities);
		testFilterEntities(varEntityClauseResult, {}, varEntities);
	}

	SECTION("One RelationshipClauseResult with match") {
		PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				stmtArg, varArg, {firstRelationship});

		vector<PQLEntity> expectedEntities = {firstStmtEntity};
		testFilterEntities(stmtEntityClauseResult, {firstRelationshipsResult}, expectedEntities);

		expectedEntities = {firstVarEntity};
		testFilterEntities(varEntityClauseResult, {firstRelationshipsResult}, expectedEntities);
	}

	SECTION("One RelationshipClauseResult with no match") {
		PQLRelationship firstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {firstRelationship});

		vector<PQLEntity> expectedEntities = stmtEntities;
		testFilterEntities(stmtEntityClauseResult, {firstRelationshipsResult}, expectedEntities);
	}

	SECTION("Multiple RelationshipClauseResults with match") {
		PQLRelationship firstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {firstRelationship});

		PQLRelationship secondRelationship = PQLRelationship(firstStmtEntity, firstProcEntity);
		PQLRelationship thirdRelationship = PQLRelationship(secondStmtEntity, firstProcEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				stmtArg, procArg, {secondRelationship, thirdRelationship});

		PQLRelationship fourthRelationship = PQLRelationship(firstStmtEntity, firstProcEntity);
		RelationshipClauseResult thirdRelationshipsResult = RelationshipClauseResult(
				stmtArg, procArg, {fourthRelationship});

		vector<PQLEntity> expectedEntities = {firstStmtEntity};
		testFilterEntities(stmtEntityClauseResult,
						   {firstRelationshipsResult, secondRelationshipsResult, thirdRelationshipsResult},
						   expectedEntities);
	}

	SECTION("Multiple RelationshipClauseResults with no match") {
		PQLRelationship firstRelationship = PQLRelationship(firstProcEntity, firstConstEntity);
		RelationshipClauseResult firstRelationshipsResult = RelationshipClauseResult(
				procArg, constArg, {firstRelationship});

		PQLRelationship secondRelationship = PQLRelationship(readEntity, firstProcEntity);
		PQLRelationship thirdRelationship = PQLRelationship(readEntity, secondProcEntity);
		RelationshipClauseResult secondRelationshipsResult = RelationshipClauseResult(
				readArg, procArg, {secondRelationship, thirdRelationship});

		vector<PQLEntity> expectedEntities = stmtEntities;
		testFilterEntities(stmtEntityClauseResult,
						   {firstRelationshipsResult, secondRelationshipsResult},
						   expectedEntities);
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
        ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
        PQLEntity varEntity = PQLEntity::generateVariable("x");
        shared_ptr<EntityClauseResult> entityClauseResultWithVariable =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						variableArg,vector<PQLEntity>{varEntity}));

        set<string> expectedSet = set<string>{"x"};

        testCombine(entityClauseResultWithVariable,
					list<shared_ptr<RelationshipClauseResult>>{},
					expectedSet);

        // x = x + 1; constant c; Select c
        ClauseArgument constantArg = ClauseArgument::createConstantArg("c");
        PQLEntity constEntity = PQLEntity::generateConstant(1);
        shared_ptr<EntityClauseResult> entityClauseResultWithConstant =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						constantArg,vector<PQLEntity>{constEntity}));

        expectedSet = set<string>{"1"};

        testCombine(entityClauseResultWithConstant,
					list<shared_ptr<RelationshipClauseResult>>{},
					expectedSet);

        // x = x + 1; procedure p; Select p
        ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
        shared_ptr<EntityClauseResult> entityClauseResultWithProcedure =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						procArg,vector<PQLEntity>{}));

        expectedSet = set<string>{};

        testCombine(entityClauseResultWithProcedure,
					list<shared_ptr<RelationshipClauseResult>>{},
					expectedSet);
    }

    SECTION("SelectClause with ModifiesSClause") {

        ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
        ClauseArgument varArg = ClauseArgument::createVariableArg("v");

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
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
			1. read y;
			2. x = y;
			3. while (x > 0) {
			4.	   print z;
			5.	   if (y > x) {
			6.     	    x = x - 1;
				   }
			   }
			7. if (y < 5) then {
			8.     y = 3;
			   } else {
			9.     y = z;
			   }
		}
	*/

	// Initialise statements
	Entity readEntity = Entity::createReadEntity(1);
	Entity a2Entity = Entity::createAssignEntity(2);
	Entity whileEntity = Entity::createWhileEntity(3);
	Entity printEntity = Entity::createPrintEntity(4);
	Entity if5Entity = Entity::createIfEntity(5);
	Entity a6Entity = Entity::createAssignEntity(6);
	Entity if7Entity = Entity::createIfEntity(7);
	Entity a8Entity = Entity::createAssignEntity(8);
	Entity a9Entity = Entity::createAssignEntity(9);

	// Initialise entities
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity y = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("y"));
	Entity x = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x"));
	Entity zeroEntity = Entity::createConstantEntity(3, Token::createIntegerToken("0"));
	Entity z = Entity::createVariableEntity(4, Token::createNameOrKeywordToken("z"));
	Entity oneEntity = Entity::createConstantEntity(6, Token::createIntegerToken("1"));
	Entity fiveEntity = Entity::createConstantEntity(7, Token::createIntegerToken("5"));
	Entity threeEntity = Entity::createConstantEntity(8, Token::createIntegerToken("3"));

	// Initialise follows relationships
	Relationship followsReadA2 = Relationship::createFollowsRelationship(readEntity, a2Entity);
	Relationship followsA2While = Relationship::createFollowsRelationship(a2Entity, whileEntity);
	Relationship followsPrintIf5 = Relationship::createFollowsRelationship(printEntity, if5Entity);
	Relationship followsWhileIf7 = Relationship::createFollowsRelationship(whileEntity, if7Entity);

	Relationship followsStarReadA2 = Relationship::createFollowsTRelationship(readEntity, a2Entity);
	Relationship followsStarReadWhile = Relationship::createFollowsTRelationship(readEntity, whileEntity);
	Relationship followsStarReadIf7 = Relationship::createFollowsTRelationship(readEntity, if7Entity);

	Relationship followsStarA2While = Relationship::createFollowsTRelationship(a2Entity, whileEntity);
	Relationship followsStarA2If7 = Relationship::createFollowsTRelationship(a2Entity, if7Entity);

	Relationship followsStarWhileIf7 = Relationship::createFollowsTRelationship(whileEntity, if7Entity);

	Relationship followsStarPrintIf5 = Relationship::createFollowsTRelationship(printEntity, if5Entity);

	// Initialise parent relationships
	Relationship parentWhilePrint = Relationship::createParentRelationship(whileEntity, printEntity);
	Relationship parentWhileIf5 = Relationship::createParentRelationship(whileEntity, if5Entity);
	Relationship parentIf5A6 = Relationship::createParentRelationship(if5Entity, a6Entity);
	Relationship parentIf7A8 = Relationship::createParentRelationship(if7Entity, a8Entity);
	Relationship parentIf7A9 = Relationship::createParentRelationship(if7Entity, a9Entity);

	Relationship parentStarWhilePrint = Relationship::createParentTRelationship(whileEntity, printEntity);
	Relationship parentStarWhileIf5 = Relationship::createParentTRelationship(whileEntity, if5Entity);
	Relationship parentStarWhileA6 = Relationship::createParentTRelationship(whileEntity, a6Entity);

	Relationship parentStarIf5A6 = Relationship::createParentTRelationship(if5Entity, a6Entity);

	Relationship parentStarIf7A8 = Relationship::createParentTRelationship(if7Entity, a8Entity);
	Relationship parentStarIf7A9 = Relationship::createParentTRelationship(if7Entity, a9Entity);

	// Initialise modifies relationships
	Relationship modifiesSReadY = Relationship::createModifiesRelationship(readEntity, y);
	Relationship modifiesSA2X = Relationship::createModifiesRelationship(a2Entity, x);
	Relationship modifiesSA6X = Relationship::createModifiesRelationship(a6Entity, x);
	Relationship modifiesSA8Y = Relationship::createModifiesRelationship(a8Entity, y);
	Relationship modifiesSA9Y = Relationship::createModifiesRelationship(a9Entity, y);

	Relationship modifiesSWhileX = Relationship::createModifiesRelationship(whileEntity, x);
	Relationship modifiesSIf5X = Relationship::createModifiesRelationship(if5Entity, x);
	Relationship modifiesSIf7Y = Relationship::createModifiesRelationship(if7Entity, y);

	Relationship modifiesPMainY = Relationship::createModifiesRelationship(main, y);
	Relationship modifiesPMainX = Relationship::createModifiesRelationship(main, x);

	// Initialise uses relationships
	Relationship usesSA2Y = Relationship::createUsesRelationship(a2Entity, y);
	Relationship usesSPrintZ = Relationship::createUsesRelationship(printEntity, z);
	Relationship usesSA6X = Relationship::createUsesRelationship(a6Entity, x);
	Relationship usesSA6One = Relationship::createUsesRelationship(a6Entity, oneEntity);
	Relationship usesSA8Three = Relationship::createUsesRelationship(a8Entity, threeEntity);
	Relationship usesSA9Z = Relationship::createUsesRelationship(a9Entity, z);

	Relationship usesSWhileX = Relationship::createUsesRelationship(whileEntity, x);
	Relationship usesSWhileZero = Relationship::createUsesRelationship(whileEntity, zeroEntity);
	Relationship usesSIf5Y = Relationship::createUsesRelationship(if5Entity, y);
	Relationship usesSIf5X = Relationship::createUsesRelationship(if5Entity, x);
	Relationship usesSIf7Y = Relationship::createUsesRelationship(if7Entity, y);
	Relationship usesSIf7Five = Relationship::createUsesRelationship(if7Entity, fiveEntity);

	Relationship usesPMainY = Relationship::createUsesRelationship(main, y);
	Relationship usesPMainX = Relationship::createUsesRelationship(main, x);
	Relationship usesPMainZero = Relationship::createUsesRelationship(main, zeroEntity);
	Relationship usesPMainZ = Relationship::createUsesRelationship(main, z);
	Relationship usesPMainOne = Relationship::createUsesRelationship(main, oneEntity);
	Relationship usesPMainFive = Relationship::createUsesRelationship(main, fiveEntity);
	Relationship usesPMainThree = Relationship::createUsesRelationship(main, threeEntity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlI5 = PQLEntity::generateStatement(5);
	PQLEntity pqlA6 = PQLEntity::generateStatement(6);
	PQLEntity pqlI7 = PQLEntity::generateStatement(7);
	PQLEntity pqlA8 = PQLEntity::generateStatement(8);
	PQLEntity pqlA9 = PQLEntity::generateStatement(9);

	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlX = PQLEntity::generateVariable("x");
	PQLEntity pqlY = PQLEntity::generateVariable("y");
	PQLEntity pqlZ = PQLEntity::generateVariable("z");
	PQLEntity pql0 = PQLEntity::generateConstant(0);
	PQLEntity pql1 = PQLEntity::generateConstant(1);
	PQLEntity pql3 = PQLEntity::generateConstant(3);
	PQLEntity pql5 = PQLEntity::generateConstant(5);

	PQLRelationship pqlFollowsR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsP4I5 = PQLRelationship(pqlP4, pqlI5);
	PQLRelationship pqlFollowsW3I7 = PQLRelationship(pqlW3, pqlI7);

	PQLRelationship pqlFollowsStarR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsStarR1W3 = PQLRelationship(pqlR1, pqlW3);
	PQLRelationship pqlFollowsStarR1I7 = PQLRelationship(pqlR1, pqlI7);

	PQLRelationship pqlFollowsStarA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsStarA2I7 = PQLRelationship(pqlA2, pqlI7);

	PQLRelationship pqlFollowsStarP4I5 = PQLRelationship(pqlP4, pqlI5);

	PQLRelationship pqlFollowsStarW3I7 = PQLRelationship(pqlW3, pqlI7);

	PQLRelationship pqlParentW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlParentW3I5 = PQLRelationship(pqlW3, pqlI5);
	PQLRelationship pqlParentI5A6 = PQLRelationship(pqlI5, pqlA6);
	PQLRelationship pqlParentI7A8 = PQLRelationship(pqlI7, pqlA8);
	PQLRelationship pqlParentI7A9 = PQLRelationship(pqlI7, pqlA9);

	PQLRelationship pqlParentStarW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlParentStarW3I5 = PQLRelationship(pqlW3, pqlI5);
	PQLRelationship pqlParentStarW3A6 = PQLRelationship(pqlW3, pqlA6);

	PQLRelationship pqlParentStarI5A6 = PQLRelationship(pqlI5, pqlA6);

	PQLRelationship pqlParentStarI7A8 = PQLRelationship(pqlI7, pqlA8);
	PQLRelationship pqlParentStarI7A9 = PQLRelationship(pqlI7, pqlA9);

	PQLRelationship pqlModifiesSR1Y = PQLRelationship(pqlR1, pqlY);
	PQLRelationship pqlModifiesSA2X = PQLRelationship(pqlA2, pqlX);
	PQLRelationship pqlModifiesSA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlModifiesSA8Y = PQLRelationship(pqlA8, pqlY);
	PQLRelationship pqlModifiesSA9Y = PQLRelationship(pqlA9, pqlY);

	PQLRelationship pqlModifiesSW3X = PQLRelationship(pqlW3, pqlX);
	PQLRelationship pqlModifiesSI5X = PQLRelationship(pqlI5, pqlX);
	PQLRelationship pqlModifiesSI7Y = PQLRelationship(pqlA9, pqlY);

	PQLRelationship pqlModifiesPMainX = PQLRelationship(pqlMain, pqlX);
	PQLRelationship pqlModifiesPMainY = PQLRelationship(pqlMain, pqlY);

	PQLRelationship pqlUsesSA2Y = PQLRelationship(pqlA2, pqlY);
	PQLRelationship pqlUsesSP4Z = PQLRelationship(pqlP4, pqlZ);
	PQLRelationship pqlUsesSA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlUsesSA6One = PQLRelationship(pqlA6, pql1);
	PQLRelationship pqlUsesSA8Three = PQLRelationship(pqlA8, pql3);
	PQLRelationship pqlUsesSA9Z = PQLRelationship(pqlA9, pqlZ);

	PQLRelationship pqlUsesSW3X = PQLRelationship(pqlW3, pqlX);
	PQLRelationship pqlUsesSW3Zero = PQLRelationship(pqlW3, pql0);
	PQLRelationship pqlUsesSI5Y = PQLRelationship(pqlI5, pqlY);
	PQLRelationship pqlUsesSI5X = PQLRelationship(pqlI5, pqlX);
	PQLRelationship pqlUsesSI7Y = PQLRelationship(pqlI7, pqlY);
	PQLRelationship pqlUsesSI7Five = PQLRelationship(pqlI7, pql5);

	PQLRelationship pqlUsesPMainY = PQLRelationship(pqlMain, pqlY);
	PQLRelationship pqlUsesPMainX = PQLRelationship(pqlMain, pqlZ);
	PQLRelationship pqlUsesPMainZero = PQLRelationship(pqlMain, pql0);
	PQLRelationship pqlUsesPMainZ = PQLRelationship(pqlMain, pqlZ);
	PQLRelationship pqlUsesPMainOne = PQLRelationship(pqlMain, pql1);
	PQLRelationship pqlUsesPMainFive = PQLRelationship(pqlMain, pql5);
	PQLRelationship pqlUsesPMainThree = PQLRelationship(pqlMain, pql3);

	// Pattern
	Pattern a2Pattern = Pattern::createAssignPattern(2, "x", "y");
	Pattern a6Pattern = Pattern::createAssignPattern(6, "x", "x1-");
	Pattern a8Pattern = Pattern::createAssignPattern(8, "y", "3");
	Pattern a9Pattern = Pattern::createAssignPattern(9, "y", "z");

	PQLRelationship pqlPatternA2X = PQLRelationship(pqlA2, pqlX);
	PQLRelationship pqlPatternA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlPatternA8Y = PQLRelationship(pqlA8, pqlY);
	PQLRelationship pqlPatternA9Y = PQLRelationship(pqlA9, pqlY);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s1");

	ClauseArgument XStringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument YStringLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument nonExistentStringLiteralArg = ClauseArgument::createStringLiteralArg("z");

	ClauseArgument a2PatternStringArg = ClauseArgument::createPatternStringArg("y");
	ClauseArgument a6PatternStringArg = ClauseArgument::createPatternStringArg("x1-");
	ClauseArgument a8PatternStringArg = ClauseArgument::createPatternStringArg("3");
	ClauseArgument a9PatternStringArg = ClauseArgument::createPatternStringArg("z");
	ClauseArgument nonExistentPatternStringArg = ClauseArgument::createPatternStringArg("aAaAaA");

	ClauseArgument a2PatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_y_");
	ClauseArgument a6FirstPatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_x1-_");
	ClauseArgument a6SecondPatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_x_");
	ClauseArgument a6ThirdPatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_1_");

}

TEST_CASE("QueryEvaluator: test evaluate") {
	auto testEvaluate = [](Query query,
						  set<string> expectedSet,
						  shared_ptr<PKB> pkb) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();
		shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

		// when
		set<string> actualSet = evaluator.evaluate(query, pkbInterface);

		// then
		REQUIRE(actualSet == expectedSet);

	};

	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, if5Entity, a6Entity, if7Entity, a8Entity, a9Entity,
							 main, x, y, z, zeroEntity, oneEntity, threeEntity, fiveEntity };

	vector<Relationship> relationships{ followsReadA2, followsA2While, followsPrintIf5, followsWhileIf7, followsStarReadA2,
										followsStarReadWhile, followsStarReadIf7, followsStarA2While, followsStarA2If7,
										followsStarWhileIf7, followsStarPrintIf5,

										parentWhilePrint, parentWhileIf5, parentIf5A6, parentIf7A8, parentIf7A9,
										parentStarWhilePrint, parentStarWhileIf5, parentStarWhileA6, parentStarIf5A6,
										parentStarIf7A8, parentStarIf7A9,

										modifiesSReadY, modifiesSA2X, modifiesSA6X, modifiesSA8Y, modifiesSA9Y,
										modifiesSWhileX, modifiesSIf5X, modifiesSIf7Y, modifiesPMainY, modifiesPMainX,

										usesSA2Y, usesSPrintZ, usesSA6X, usesSA6One, usesSA8Three, usesSA9Z, usesSWhileX,
										usesSWhileZero, usesSIf5Y, usesSIf5X, usesSIf7Y, usesSIf7Five, usesPMainY,
										usesPMainX, usesPMainZero, usesPMainZ, usesPMainOne, usesPMainFive, usesPMainThree};

	vector<Pattern> patterns{ a2Pattern , a6Pattern , a8Pattern , a9Pattern };

	// ------ PKB ------
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);
	pkb->addPatterns(patterns);

	// ------ QPS ------
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(stmtArg);
	Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
	set<string> expectedSet = set<string>{"1", "2", "3", "4", "5", "6", "7", "8", "9"};

	SECTION("Select only") {
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(readArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(printArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(assignArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(whileArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(ifArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(procArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(constArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"0", "1", "3", "5"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Follows") {

		// Select s1 such that Follows(s1, s2)
		selectClause = make_shared<SelectClause>(stmtArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"1", "2", "3", "4"};
		testEvaluate(query, expectedSet, pkb);

		// Select s2 such that Follows(s1, s2)
		selectClause = make_shared<SelectClause>(secondStmtArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "3", "5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s1 such that Follows*(s1, s2)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"1", "2", "3", "4"};
		testEvaluate(query, expectedSet, pkb);

		// Select s2 such that Follows*(s1, s2)
		selectClause = make_shared<SelectClause>(secondStmtArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "3", "5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(r, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(readArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(r, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(readArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "3", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(a, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(assignArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(a, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(assignArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(w, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(w, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"7"};
		testEvaluate(query, expectedSet, pkb);

	}

	SECTION("Select + Parent") {

		// Select s1 such that Parent(s1, s2)
		selectClause = make_shared<SelectClause>(stmtArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3", "5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s2 such that Parent(s1, s2)
		selectClause = make_shared<SelectClause>(secondStmtArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"4", "5", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select i such that Parent(i, s)
		selectClause = make_shared<SelectClause>(ifArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Parent(i, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select w such that Parent(w, s)
		selectClause = make_shared<SelectClause>(whileArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Parent(w, s)
		selectClause = make_shared<SelectClause>(stmtArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"4", "5"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Parent*(w, s)
		relationshipClause = shared_ptr<RelationshipClause>(new ParentTClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"4", "5", "6"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Modifies") {

		// Select s such that Modifies(s, v)
		selectClause = make_shared<SelectClause>(stmtArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(stmtArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"1", "2", "3", "5", "6", "7", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(s, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select r such that Modifies(r, v)
		selectClause = make_shared<SelectClause>(readArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(readArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(r, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"y"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, v)
		selectClause = make_shared<SelectClause>(assignArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(a, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select w such that Modifies(w, v)
		selectClause = make_shared<SelectClause>(whileArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(whileArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(w, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);

		// Select i such that Modifies(i, v)
		selectClause = make_shared<SelectClause>(ifArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(ifArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(i, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr such that Modifies(pr, v)
		selectClause = make_shared<SelectClause>(procArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesPClause(procArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(pr, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

	}

	SECTION("Select + Uses") {

		// Select s such that Uses(s, v)
		selectClause = make_shared<SelectClause>(stmtArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(stmtArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "3", "4", "5", "6", "7", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(s, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		// Select p such that Uses(p, v)
		selectClause = make_shared<SelectClause>(printArg);
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(printArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(p, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"z"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Uses(a, v)
		selectClause = make_shared<SelectClause>(assignArg);
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(assignArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"2", "6", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(a, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		// Select w such that Uses(w, v)
		selectClause = make_shared<SelectClause>(whileArg);
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(whileArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(w, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);

		// Select i such that Uses(i, v)
		selectClause = make_shared<SelectClause>(ifArg);
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(ifArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(i, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr such that Uses(pr, v)
		selectClause = make_shared<SelectClause>(procArg);
		relationshipClause = shared_ptr<RelationshipClause>(new UsesPClause(procArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(pr, v)
		selectClause = make_shared<SelectClause>(varArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{});
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

	}

	SECTION("Select + Assign Pattern") {
		// Select a pattern a(v, _)
		selectClause = make_shared<SelectClause>(assignArg);
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(_, _)
		selectClause = make_shared<SelectClause>(assignArg);
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, wildcardArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v pattern a(v, _)
		selectClause = make_shared<SelectClause>(varArg);
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a("x", _)
		selectClause = make_shared<SelectClause>(assignArg);
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, XStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2", "6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a("y", _)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, YStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a("z", _)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "y")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a2PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "x - 1")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "3")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a8PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"8"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "z")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a9PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "aAaAaA")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, nonExistentPatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"y"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a2PatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"x1-"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6FirstPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"x"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6SecondPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"1"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6ThirdPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - non-empty results, two matching clauses") {
		// Select a such that Modifies(a, v) pattern a(v, _)
		selectClause = make_shared<SelectClause>(assignArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(a, v) pattern a(v, _)
		selectClause = make_shared<SelectClause>(varArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, v) pattern a(v, _"x"_)
		selectClause = make_shared<SelectClause>(assignArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6SecondPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(a, v) pattern a(v, _"x"_)
		selectClause = make_shared<SelectClause>(varArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6SecondPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);
	}


	SECTION("Select + Such That + Pattern - non-empty results, one matching clause") {
		// Select a such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(assignArg);
		ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
		ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("y");
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(lineNumArg, stringLiteralArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(1, "y") pattern a("x", _)
		selectClause = make_shared<SelectClause>(assignArg);
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, XStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"2", "6"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - non-empty results, no matching clause") {
		// Select r such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(readArg);
		ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
		ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("y");
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(lineNumArg, stringLiteralArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select p such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(printArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(procArg);
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, select empty") {
		// Select c such that Modifies(a, v) pattern a(v, _)
		ClauseArgument callArg = ClauseArgument::createCallArg("c");
		selectClause = make_shared<SelectClause>(callArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, no tables empty") {
		// Select a such that Modifies(a, "y") pattern a("x", _)
		selectClause = make_shared<SelectClause>(assignArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, YStringLiteralArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, XStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, "x") pattern a("y", _)
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, XStringLiteralArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, YStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, "x") pattern a(v, "z")
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, XStringLiteralArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a9PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Follows*(a, s) pattern a(v, _"3"_)
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(assignArg, stmtArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a8PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, one relationship table empty") {
		// Select a such that Follows*(i, a) pattern a(v, _)
		selectClause = make_shared<SelectClause>(assignArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, assignArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, v) pattern a("z", _)
		selectClause = make_shared<SelectClause>(assignArg);
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, two relationship tables empty") {
		// Select a such that Follows*(i, a) pattern a("z", _)
		selectClause = make_shared<SelectClause>(assignArg);
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, assignArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}
}


