#include "catch.hpp"
#include <qps/query_evaluator/QueryResultsOptimiser.h>

using namespace std;

// ==================== UNIT TESTS ====================

TEST_CASE("QueryResultsOptimiser: test optimise") {
	auto testCombine = [](vector<ClauseResult> selectResults,
			vector<ClauseResult> relationshipResults, vector<ClauseResult> withResults, vector<vector<ClauseResult>> expected) {
		// given
		QueryResultsOptimiser optimiser = QueryResultsOptimiser(selectResults, relationshipResults, withResults);

		// when
		vector<vector<ClauseResult>> actual = optimiser.optimise();

		// then
		REQUIRE(actual == expected);
	};

	// TODO: Write tests for optimise
}