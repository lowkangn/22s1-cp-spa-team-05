#pragma once

#include <list>

#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsCombiner {
private:
	vector<ClauseResult> selectResults;
	vector<vector<ClauseResult>> optimisedConstraintResults;

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
	QueryResultsCombiner(vector<ClauseResult> selectResults, vector<vector<ClauseResult>> optimisedConstraintResults) {
		this->selectResults = selectResults;
		this->optimisedConstraintResults = optimisedConstraintResults;
	}

	ClauseResult combine();
};
