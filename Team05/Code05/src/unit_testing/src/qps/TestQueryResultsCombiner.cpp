#include "catch.hpp"
#include <qps/query_evaluator/QueryResultsCombiner.h>

using namespace std;

// ==================== UNIT TESTS ====================

TEST_CASE("QueryResultsCombiner: test combine") {
	auto testCombine = [](vector<ClauseResult> selectResults,
			vector<vector<ClauseResult>> optimisedConstraintResults, ClauseResult expected) {
		// given
		QueryResultsCombiner combiner = QueryResultsCombiner(selectResults, optimisedConstraintResults);

		// when
		ClauseResult actual = combiner.combine();

		// then

		REQUIRE(actual == expected);
	};

	// TODO: Write tests for combine
}
