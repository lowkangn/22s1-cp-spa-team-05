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

	ClauseArgument stmtArg = ClauseArgument("s", ArgumentType::STMT);
	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
	ClauseArgument constArg = ClauseArgument("c", ArgumentType::CONSTANT);
	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
	ClauseArgument readArg = ClauseArgument("r", ArgumentType::READ);

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

		vector<vector<PQLEntity>> expectedCombinedTable = {};
		vector<ClauseArgument> expectedArguments = {stmtArg, varArg, constArg};

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
