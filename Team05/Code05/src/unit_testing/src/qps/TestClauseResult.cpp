#include "catch.hpp"
#include <qps/query/clause/ClauseResult.h>

using namespace std;

// =============== UNIT TESTS ====================

namespace {
	// Initialise variable argument and entities
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	PQLEntity varX = PQLEntity::generateVariable("x");
	PQLEntity varY = PQLEntity::generateVariable("y");
	PQLEntity varZ = PQLEntity::generateVariable("z");

	// Initialise statement argument and entities
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument callArg = ClauseArgument::createCallArg("ca");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	PQLEntity stmt1 = PQLEntity::generateStatement(1);
	PQLEntity stmt2 = PQLEntity::generateStatement(2);
	PQLEntity stmt3 = PQLEntity::generateStatement(3);
	PQLEntity stmt4 = PQLEntity::generateStatement(4);
	PQLEntity stmt5 = PQLEntity::generateStatement(5);

	// Initialise constant argument and entities
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	PQLEntity const11 = PQLEntity::generateConstant(11);
	PQLEntity const12 = PQLEntity::generateConstant(12);
	PQLEntity const13 = PQLEntity::generateConstant(13); 
	PQLEntity const3 = PQLEntity::generateConstant(3);

	// Initialise procedure argument and entities
	ClauseArgument procArg = ClauseArgument::createProcedureArg("pr");
	PQLEntity procMain = PQLEntity::generateProcedure("main");
	PQLEntity procX = PQLEntity::generateProcedure("x");

	// Initialise some attribute arguments
	ClauseArgument callProcNameArg = ClauseArgument::createProcNameAttributeArg(callArg);
	ClauseArgument readVarNameArg = ClauseArgument::createVarNameAttributeArg(readArg);

	// Create result from selecting all variables: `Select v`
	vector<ClauseArgument> selectVarArg = {varArg};
	Table selectVarResultTable = {
			{varX},
			{varY},
			{varZ}
	};
	ClauseResult selectVarResult = ClauseResult(selectVarArg, selectVarResultTable);

	// Create result from selecting all statements: `Select s`
	vector<ClauseArgument> selectStmtArg = {stmtArg};
	Table selectStmtResultTable = {
			{stmt1},
			{stmt2},
			{stmt3},
			{stmt4},
			{stmt5}
	};
	ClauseResult selectStmtResult = ClauseResult(selectStmtArg, selectStmtResultTable);

	// Create result from selecting all constants: `Select c`
	vector<ClauseArgument> selectConstArg = {constArg};
	Table selectConstResultTable = {
			{const11},
			{const12},
			{const13},
			{const3},
	};
	ClauseResult selectConstResult = ClauseResult(selectConstArg, selectConstResultTable);

	// Create result from selecting all procedure: `Select pr`
	vector<ClauseArgument> selectProcArg = {procArg};
	Table selectProcResultTable = {
			{procMain},
			{procX},
	};
	ClauseResult selectProcResult = ClauseResult(selectProcArg, selectProcResultTable);
}

TEST_CASE("ClauseResult: test mergeResult") {
	auto testMergeResult = [](ClauseResult first, ClauseResult second, ClauseResult expected) {
		// when
		ClauseResult actual = first.mergeResult(second);

		// then
		REQUIRE(actual.equals(make_shared<ClauseResult>(expected)));
	};

	vector<ClauseArgument> firstResultArgs = {stmtArg, varArg};
	Table firstResultTable = {
			{stmt1, varX}
	};
	ClauseResult first = ClauseResult(firstResultArgs, firstResultTable);

	vector<ClauseArgument> secondResultArgs = {procArg, varArg};
	Table secondResultTable = {
			{procMain, varY},
			{procMain, varZ}
	};
	ClauseResult second = ClauseResult(secondResultArgs, secondResultTable);

	vector<ClauseArgument> combinedResultArgs = {stmtArg, varArg, procArg};
	Table combinedResultTable = {};
	ClauseResult combined = ClauseResult(combinedResultArgs, combinedResultTable);

	SECTION("Inner join - empty") {
		testMergeResult(first, second, combined);
	}

	SECTION("Inner join - non-empty") {
		firstResultArgs = {stmtArg, varArg};
		firstResultTable = {
				{stmt1, varX},
				{stmt2, varY},
				{stmt3, varZ}
		};
		first = ClauseResult(firstResultArgs, firstResultTable);

		secondResultArgs = {procArg, varArg};
		secondResultTable = {
				{procMain, varX},
				{procMain, varY}
		};
		second = ClauseResult(secondResultArgs, secondResultTable);

		combinedResultArgs = {stmtArg, varArg, procArg};
		combinedResultTable = {
				{stmt1, varX, procMain},
				{stmt2, varY, procMain}
		};
		combined = ClauseResult(combinedResultArgs, combinedResultTable);

		testMergeResult(first, second, combined);
	}

	SECTION("Cross product") {
		firstResultArgs = {stmtArg, varArg};
		firstResultTable = {
				{stmt1, varX},
				{stmt2, varY},
				{stmt3, varZ}
		};
		first = ClauseResult(firstResultArgs, firstResultTable);

		secondResultArgs = {procArg, constArg};
		secondResultTable = {
				{procMain, const11},
				{procMain, const12},
				{procMain, const13}
		};
		second = ClauseResult(secondResultArgs, secondResultTable);

		combinedResultArgs = {stmtArg, varArg, procArg, constArg};
		combinedResultTable = {
				{stmt1, varX, procMain, const11},
				{stmt1, varX, procMain, const12},
				{stmt1, varX, procMain, const13},
				{stmt2, varY, procMain, const11},
				{stmt2, varY, procMain, const12},
				{stmt2, varY, procMain, const13},
				{stmt3, varZ, procMain, const11},
				{stmt3, varZ, procMain, const12},
				{stmt3, varZ, procMain, const13}
		};
		combined = ClauseResult(combinedResultArgs, combinedResultTable);

		testMergeResult(first, second, combined);
	}
};

TEST_CASE("ClauseResult: test checkSelectArgsInTable") {
	auto testCheckSelectArgsInTable = [](ClauseResult result, vector<ClauseResult> selectResults, bool expected) {
		// when
		bool actual = result.checkSelectArgsInTable(selectResults);

		// then
		REQUIRE(actual == expected);
	};

	vector<ClauseArgument> resultArgs = {stmtArg, varArg};
	Table resultTable = {
			{stmt1, varX},
			{stmt2, varY}
	};
	ClauseResult result = ClauseResult(resultArgs, resultTable);

	SECTION("At least one arg found in table") {
		testCheckSelectArgsInTable(result, {selectStmtResult}, true);
		testCheckSelectArgsInTable(result, {selectVarResult}, true);
		testCheckSelectArgsInTable(result, {selectStmtResult, selectVarResult}, true);
		testCheckSelectArgsInTable(result, {selectStmtResult, selectConstResult}, true);
		testCheckSelectArgsInTable(result, {selectProcResult, selectVarResult}, true);
	}

	SECTION("No args found in table") {
		testCheckSelectArgsInTable(result, {selectProcResult}, false);
		testCheckSelectArgsInTable(result, {selectConstResult}, false);
		testCheckSelectArgsInTable(result, {selectProcResult, selectConstResult}, false);
	}
};

TEST_CASE("ClauseResult: test rearrangeTableToMatchSelectResults") {
	auto testRearrangeTableToMatchSelectResults = [](ClauseResult result, vector<ClauseResult> selectResults, ClauseResult expected) {
		// when
		ClauseResult actual = result.rearrangeTableToMatchSelectResults(selectResults);

		// then
		REQUIRE(actual.equals(make_shared<ClauseResult>(expected)));
	};

	vector<ClauseArgument> resultArgs = {stmtArg, varArg, procArg};
	Table resultTable = {
			{stmt1, varX, procMain},
			{stmt2, varY, procMain}
	};
	ClauseResult result = ClauseResult(resultArgs, resultTable);

	SECTION("All select args in table") {
		vector<ClauseResult> selectResults = {selectStmtResult};
		vector<ClauseArgument> expectedResultArgs = {stmtArg};
		Table expectedResultTable = {
				{stmt1},
				{stmt2}
		};
		ClauseResult expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectVarResult};
		expectedResultArgs = {varArg};
		expectedResultTable = {
				{varX},
				{varY}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectProcResult};
		expectedResultArgs = {procArg};
		expectedResultTable = {
				{procMain},
				{procMain}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectStmtResult, selectVarResult};
		expectedResultArgs = {stmtArg, varArg};
		expectedResultTable = {
				{stmt1, varX},
				{stmt2, varY}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectProcResult, selectVarResult};
		expectedResultArgs = {procArg, varArg};
		expectedResultTable = {
				{procMain, varX},
				{procMain, varY}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectStmtResult, selectStmtResult};
		expectedResultArgs = {stmtArg, stmtArg};
		expectedResultTable = {
				{stmt1, stmt1},
				{stmt2, stmt2}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectStmtResult, selectVarResult, selectStmtResult};
		expectedResultArgs = {stmtArg, varArg, stmtArg};
		expectedResultTable = {
				{stmt1, varX, stmt1},
				{stmt2, varY, stmt2}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);
	}

	SECTION("Some select args not in table") {
		vector<ClauseResult> selectResults = {selectStmtResult, selectConstResult};
		vector<ClauseArgument> expectedResultArgs = {stmtArg, constArg};
		Table expectedResultTable = {
				{stmt1, const11},
				{stmt1, const12},
				{stmt1, const13},
				{stmt1, const3},
				{stmt2, const11},
				{stmt2, const12},
				{stmt2, const13},
				{stmt2, const3},
		};
		ClauseResult expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);

		selectResults = {selectStmtResult, selectVarResult, selectConstResult, selectConstResult};
		expectedResultArgs = {stmtArg, varArg, constArg, constArg};
		expectedResultTable = {
				{stmt1, varX, const11, const11},
				{stmt1, varX, const12, const12},
				{stmt1, varX, const13, const13},
				{stmt1, varX, const3, const3},
				{stmt2, varY, const11, const11},
				{stmt2, varY, const12, const12},
				{stmt2, varY, const13, const13},
				{stmt2, varY, const3, const3}
		};
		expectedResult = ClauseResult(expectedResultArgs, expectedResultTable);
		testRearrangeTableToMatchSelectResults(result, selectResults, expectedResult);
	}
};

TEST_CASE("ClauseResult: test duplicateColumn") {
	auto testDuplicateColumn = [](ClauseResult result, ClauseResult column, ClauseResult expected) {
		// when
		result.duplicateColumn(column);
		ClauseResult actual = result;

		// then
		REQUIRE(actual.equals(make_shared<ClauseResult>(expected)));
	};

	vector<ClauseArgument> resultArgs = {stmtArg, varArg, procArg};
	Table resultTable = {
			{stmt1, varX, procMain},
			{stmt2, varY, procMain}
	};
	ClauseResult result = ClauseResult(resultArgs, resultTable);

	vector<ClauseArgument> columnArg = {stmtArg};
	Table columnTable = {
			{stmt1},
			{stmt2}
	};
	ClauseResult column = ClauseResult(columnArg, columnTable);

	vector<ClauseArgument> expectedArgs = {stmtArg, varArg, procArg, stmtArg};
	Table expectedTable = {
			{stmt1, varX, procMain, stmt1},
			{stmt2, varY, procMain, stmt2}
	};
	ClauseResult expected = ClauseResult(expectedArgs, expectedTable);

	testDuplicateColumn(result, column, expected);

	columnArg = {varArg};
	columnTable = {
			{varX},
			{varY}
	};
	column = ClauseResult(columnArg, columnTable);

	expectedArgs = {stmtArg, varArg, procArg, varArg};
	expectedTable = {
			{stmt1, varX, procMain, varX},
			{stmt2, varY, procMain, varY}
	};
	expected = ClauseResult(expectedArgs, expectedTable);

	testDuplicateColumn(result, column, expected);

	columnArg = {procArg};
	columnTable = {
			{procMain}
	};
	column = ClauseResult(columnArg, columnTable);

	expectedArgs = {stmtArg, varArg, procArg, procArg};
	expectedTable = {
			{stmt1, varX, procMain, procMain},
			{stmt2, varY, procMain, procMain}
	};
	expected = ClauseResult(expectedArgs, expectedTable);

	testDuplicateColumn(result, column, expected);
};

TEST_CASE("ClauseResult: test convertTableToString") {
	auto testConvertTableToString = [](ClauseResult result, bool isBooleanReturnType, set<string> expected) {
		// when
		set<string> actual = result.convertTableToString(isBooleanReturnType);

		// then
		REQUIRE(actual == expected);
	};

	SECTION("Empty table") {
		vector<ClauseArgument> resultArgs = {};
		Table resultTable = {};
		ClauseResult result = ClauseResult(resultArgs, resultTable);

		testConvertTableToString(result, true, {"FALSE"});
		testConvertTableToString(result, false, {});
	}

	SECTION("Single column") {
		vector<ClauseArgument> resultArgs = {stmtArg};
		Table resultTable = {
				{stmt1},
				{stmt2},
				{stmt3},
				{stmt4},
				{stmt5}
		};
		ClauseResult result = ClauseResult(resultArgs, resultTable);

		set<string> expectedStrings = {"1", "2", "3", "4", "5"};

		testConvertTableToString(result, true, {"TRUE"});
		testConvertTableToString(result, false, expectedStrings);
	}

	SECTION("Multiple columns") {
		vector<ClauseArgument> resultArgs = {stmtArg, varArg, constArg, constArg};
		Table resultTable = {
				{stmt1, varX, const11, const11},
				{stmt1, varX, const12, const12},
				{stmt1, varX, const13, const13},
				{stmt2, varY, const11, const11},
				{stmt2, varY, const12, const12},
				{stmt2, varY, const13, const13}
		};
		ClauseResult result = ClauseResult(resultArgs, resultTable);

		set<string> expectedStrings = {
				"1 x 11 11",
				"1 x 12 12",
				"1 x 13 13",
				"2 y 11 11",
				"2 y 12 12",
				"2 y 13 13"
		};

		testConvertTableToString(result, true, {"TRUE"});
		testConvertTableToString(result, false, expectedStrings);
	}
};

TEST_CASE("ClauseResult: test equals") {
	auto testEquals = [](ClauseResult first, ClauseResult second, bool expected) {
		// when
		bool actual = first.equals(make_shared<ClauseResult>(second));

		// then
		REQUIRE(actual == expected);
	};

	ClauseResult anotherSelectVarResult = ClauseResult(selectVarArg, selectVarResultTable);

	SECTION("Equal") {
		testEquals(selectVarResult, selectVarResult, true);
		testEquals(selectVarResult, anotherSelectVarResult, true);

		// same arg, different table - different rows
		Table diffSelectVarResultTable = {
				{varX},
				{varZ},
				{varY}
		};
		ClauseResult diffTableSelectVarResult = ClauseResult(selectVarArg, diffSelectVarResultTable);
		testEquals(selectVarResult, diffTableSelectVarResult, true);
	}

	SECTION("Not equal") {
		// different arg types
		testEquals(selectVarResult, selectStmtResult, false);
		testEquals(selectVarResult, selectProcResult, false);
		testEquals(selectVarResult, selectConstResult, false);

		// same arg type, different arg
		ClauseArgument anotherVarArg = ClauseArgument::createVariableArg("v1");
		vector<ClauseArgument> anotherSelectVarArg = {anotherVarArg};
		ClauseResult diffArgSelectVarResult = ClauseResult(anotherSelectVarArg, selectVarResultTable);
		testEquals(selectVarResult, diffArgSelectVarResult, false);

		// same arg, different table - different number of rows
		Table diffSelectVarResultTable = {
				{varX},
				{varY}
		};
		ClauseResult diffTableSelectVarResult = ClauseResult(selectVarArg, diffSelectVarResultTable);
		testEquals(selectVarResult, diffTableSelectVarResult, false);
	}
};

TEST_CASE("ClauseResult: test mergeByForceInnerJoin") {
	auto testMergeResult = [](ClauseResult first, ClauseResult second, 
		ClauseArgument leftOn, ClauseArgument rightOn, ClauseResult expected) {
		// when
		ClauseResult actual = first.mergeByForceInnerJoin(second, leftOn, rightOn);

		// then
		REQUIRE(actual.equals(make_shared<ClauseResult>(expected)));
	};

	vector<ClauseArgument> firstResultArgs = { stmtArg, varArg };
	Table firstResultTable = {
			{stmt1, varX}
	};
	ClauseResult first = ClauseResult(firstResultArgs, firstResultTable);

	vector<ClauseArgument> secondResultArgs = { procArg, varArg };
	Table secondResultTable = {
			{procMain, varY},
			{procMain, varX}
	};
	ClauseResult second = ClauseResult(secondResultArgs, secondResultTable);

	ClauseArgument firstOn = varArg;
	ClauseArgument secondOn = procArg;
	vector<ClauseArgument> combinedResultArgs = { stmtArg, varArg, procArg, varArg };
	Table combinedResultTable = {};
	ClauseResult combined = ClauseResult(combinedResultArgs, combinedResultTable);

	SECTION("Force Inner Join - empty") {
		testMergeResult(first, second, firstOn, secondOn, combined);
	}

	SECTION("Force Inner join - non-empty") {
		firstResultArgs = { stmtArg, varArg };
		firstResultTable = {
				{stmt1, varX},
				{stmt2, varY},
				{stmt3, varZ}
		};
		first = ClauseResult(firstResultArgs, firstResultTable);

		secondResultArgs = { procArg, varArg };
		secondResultTable = {
				{procMain, varX},
				{procX, varY},
				{procX, varX},
		};
		second = ClauseResult(secondResultArgs, secondResultTable);

		firstOn = varArg;
		secondOn = procArg;
		combinedResultArgs = { stmtArg, varArg, procArg, varArg };
		combinedResultTable = {
				{stmt1, varX, procX, varY},
				{stmt1, varX, procX, varX}
		};
		combined = ClauseResult(combinedResultArgs, combinedResultTable);

		testMergeResult(first, second, firstOn, secondOn, combined);
	}

	SECTION("Force Inner join - non-empty, with attributes") {
		firstResultArgs = { readArg, readVarNameArg };
		firstResultTable = {
				{stmt1, varX},
				{stmt2, varY},
				{stmt3, varX}
		};
		first = ClauseResult(firstResultArgs, firstResultTable);

		secondResultArgs = { callArg, callProcNameArg };
		secondResultTable = {
				{stmt4, procMain},
				{stmt5, procX},
		};
		second = ClauseResult(secondResultArgs, secondResultTable);

		firstOn = readVarNameArg;
		secondOn = callProcNameArg;
		combinedResultArgs = { readArg, readVarNameArg, callArg, callProcNameArg };
		combinedResultTable = {
				{stmt1, varX, stmt5, procX},
				{stmt3, varX, stmt5, procX},
		};
		combined = ClauseResult(combinedResultArgs, combinedResultTable);

		// Note: equivalent to with r.varName = ca.ProcName
		testMergeResult(first, second, firstOn, secondOn, combined);
	}
};