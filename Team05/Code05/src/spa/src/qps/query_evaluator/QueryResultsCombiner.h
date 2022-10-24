#pragma once

#include <list>

#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsCombiner {
private:
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

	ClauseResult getDesiredSynonymsResult(vector<ClauseResult> selectResultsList, ClauseResult result);

	ClauseResult getSelectSynonymsCrossProductResult(vector<ClauseResult> selectResultsList);

public:
	QueryResultsCombiner() {};

	ClauseResult combine(vector<ClauseResult> selectResults, vector<vector<ClauseResult>> optimisedConstraintResults);
};
