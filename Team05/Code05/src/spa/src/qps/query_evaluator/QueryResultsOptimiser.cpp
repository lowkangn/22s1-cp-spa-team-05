#include <qps/query_evaluator/QueryResultsOptimiser.h>

vector<vector<ClauseResult>> QueryResultsOptimiser::optimise() {

	if (this->relationshipsResults.empty() && this->withResults.empty()) {
		return {};
	}

	// TODO: Implement grouping and sorting
	vector<ClauseResult> results;
	results.insert(results.end(), this->relationshipsResults.begin(), this->relationshipsResults.end());
	results.insert(results.end(), this->withResults.begin(), this->withResults.end());

	return {results};
}
