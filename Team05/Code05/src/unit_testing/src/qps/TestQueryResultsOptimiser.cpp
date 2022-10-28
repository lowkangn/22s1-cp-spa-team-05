#include "catch.hpp"
#include <qps/query_evaluator/QueryResultsOptimiser.h>

using namespace std;

// ==================== UNIT TESTS ====================

TEST_CASE("QueryResultsOptimiser: test optimise") {
	auto testCombine = [](list<ClauseResult> selectResults,
			list<ClauseResult> relationshipResults, list<ClauseResult> withResults,
			vector<vector<vector<ClauseResult>>> expectedGroups, bool expectedIsEmptyResultFound) {
		// given
		QueryResultsOptimiser optimiser = QueryResultsOptimiser(selectResults, relationshipResults, withResults);
		bool actualIsEmptyResultFound;

		// when
		vector<vector<vector<ClauseResult>>> actualGroups = optimiser.optimise(actualIsEmptyResultFound);

		// then
		REQUIRE(actualGroups == expectedGroups);
		REQUIRE(actualIsEmptyResultFound == expectedIsEmptyResultFound);
	};

	// TODO: Write tests for optimise
}
