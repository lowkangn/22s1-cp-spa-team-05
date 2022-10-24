#include <qps/query_evaluator/QueryResultsOptimiser.h>

vector<vector<ClauseResult>> QueryResultsOptimiser::optimise(vector<ClauseResult> selectResults, vector<ClauseResult> relationshipResults, vector<ClauseResult> withResults) {

	if (relationshipResults.empty() && withResults.empty()) {
		return {};
	}

	// TODO: Implement grouping and sorting
	vector<ClauseResult> results;
	results.insert(results.end(), relationshipResults.begin(), relationshipResults.end());
	results.insert(results.end(), withResults.begin(), withResults.end());

	return {results};
}
