#include "catch.hpp"
#include <qps/query_evaluator/QueryResultsCombiner.h>

using namespace std;

// ==================== UNIT TESTS ====================

TEST_CASE("QueryResultsCombiner: test combine") {
	auto testCombine = [](vector<ClauseResult> selectResults,
			vector<vector<ClauseResult>> optimisedConstraintResults, ClauseResult expected) {
		// given
		QueryResultsCombiner combiner = QueryResultsCombiner();

		// when
		ClauseResult actual = combiner.combine(selectResults, optimisedConstraintResults);

		// then

		REQUIRE(actual == expected);
	};

	// TODO: Write tests for combine
}