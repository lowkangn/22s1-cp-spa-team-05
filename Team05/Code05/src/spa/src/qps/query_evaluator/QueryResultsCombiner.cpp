#include <qps/query_evaluator/QueryResultsCombiner.h>

ClauseResult QueryResultsCombiner::combineResults(vector<ClauseResult> results) {
	// Iterate and combine results
	ClauseResult combinedResult;
	for (vector<ClauseResult>::iterator resultIter = results.begin(); resultIter != results.end(); resultIter++) {
		if (resultIter == results.begin()) {
			combinedResult = *resultIter;
		} else {
			ClauseResult newCombinedResult = combinedResult.mergeResult(*resultIter);
			combinedResult = newCombinedResult;
		}
	}

	return combinedResult;
}

ClauseResult QueryResultsCombiner::getDesiredSynonymsResult(vector<ClauseResult> selectResults, ClauseResult combinedResult) {
	// If no select results, we are returning boolean, so just return table
	if (selectResults.empty()) {
		return combinedResult;
	}

	// Check if any select args in combined table - if not, do cross product with select results
	bool areSelectArgsInCombinedTable = combinedResult.checkSelectArgsInTable(selectResults);
	if (!areSelectArgsInCombinedTable) {
		return this->getSelectSynonymsCrossProductResult(selectResults);
	} else {
		return combinedResult.rearrangeTableToMatchSelectResults(selectResults);
	}
}

ClauseResult QueryResultsCombiner::getSelectSynonymsCrossProductResult(vector<ClauseResult> selectResults) {
	ClauseResult combinedResult;
	vector<ClauseResult> selectResultsWithoutAttribute;

	for (ClauseResult selectResult : selectResults) {
		combinedResult = mergeIntoCombinedIfNotInTable(combinedResult, selectResult);
	}

	return this->getDesiredSynonymsResult(selectResults, combinedResult);
}

ClauseResult QueryResultsCombiner::combine(vector<ClauseResult> selectResults, vector<vector<ClauseResult>> optimisedConstraintResults) {
	if (optimisedConstraintResults.empty()) {
		return this->getSelectSynonymsCrossProductResult(selectResults);
	}

	// Combine constraint clauses - if returns empty, then no query matches
	ClauseResult combinedResult;
	for (vector<ClauseResult> resultsGroup : optimisedConstraintResults) {
		ClauseResult groupCombinedResult = this->combineResults(resultsGroup);
		if (groupCombinedResult.isEmpty()) {
			return groupCombinedResult;
		}
		if (combinedResult.isEmpty()) {
			combinedResult = groupCombinedResult;
		} else {
			combinedResult.mergeResult(groupCombinedResult);
			if (combinedResult.isEmpty()) {
				return combinedResult;
			}
		}
	}

	// Get a table of entries matching combination specified in select clause
	return this->getDesiredSynonymsResult(selectResults, combinedResult);
}
