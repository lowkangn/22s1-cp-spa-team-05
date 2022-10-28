#pragma once

#include <list>

#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsCombiner {
private:
	vector<ClauseResult> selectResults;
	vector<vector<ClauseResult>> resultsWithSelectedArgs;
	vector<vector<ClauseResult>> resultsWithoutSelectedArgs;

	ClauseResult mergeIntoCombinedIfNotInTable(ClauseResult combinedResult, ClauseResult resultToMerge) {
		if (combinedResult.isEmpty()) {
			combinedResult = resultToMerge;
		} else {
			bool isSelectArgInCombinedResult = combinedResult.checkSelectArgsInTable({resultToMerge});
			if (!isSelectArgInCombinedResult) {
				combinedResult = combinedResult.mergeResult(resultToMerge);
			}
		}
		return combinedResult;
	}

	ClauseResult combineResults(vector<ClauseResult> results);

	ClauseResult getDesiredSynonymsResult(ClauseResult result);

	ClauseResult getSelectSynonymsCrossProductResult();

public:
	QueryResultsCombiner(list<ClauseResult> selectResults, vector<vector<vector<ClauseResult>>> optimisedConstraintResults) {
		copy(selectResults.begin(), selectResults.end(), back_inserter(this->selectResults));
		if (!optimisedConstraintResults.empty()) {
			this->resultsWithSelectedArgs = optimisedConstraintResults.front();
			this->resultsWithoutSelectedArgs = optimisedConstraintResults.back();
		}
	}

	ClauseResult combine();
};
