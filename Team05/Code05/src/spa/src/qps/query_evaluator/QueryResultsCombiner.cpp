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

	// TODO: Find solution to this "local variable may escape the function" warning
	return combinedResult;
}

ClauseResult QueryResultsCombiner::getDesiredSynonymsResult(ClauseResult combinedResult) {
	// If no select results, we are returning boolean, so just return table
	if (this->selectResults.empty()) {
		return combinedResult;
	}

	// Check if any select args in combined table - if not, do cross product with select results
	bool areSelectArgsInCombinedTable = combinedResult.checkSelectArgsInTable(this->selectResults);
	if (!areSelectArgsInCombinedTable) {
		return this->getSelectSynonymsCrossProductResult();
	} else {
		return combinedResult.rearrangeTableToMatchSelectResults(this->selectResults);
	}
}

ClauseResult QueryResultsCombiner::getSelectSynonymsCrossProductResult() {
	ClauseResult combinedResult;
	vector<ClauseResult> selectResultsWithoutAttribute;

	for (const ClauseResult& selectResult : this->selectResults) {
		combinedResult = mergeIntoCombinedIfNotInTable(combinedResult, selectResult);
	}

	return this->getDesiredSynonymsResult(combinedResult);
}

ClauseResult QueryResultsCombiner::combine() {
	if (this->optimisedConstraintResults.empty()) {
		return this->getSelectSynonymsCrossProductResult();
	}

	// Combine constraint clauses - if returns empty, then no query matches
	ClauseResult combinedResult;
	for (const vector<ClauseResult>& resultsGroup : this->optimisedConstraintResults) {
		ClauseResult groupCombinedResult = this->combineResults(resultsGroup);
		if (groupCombinedResult.isEmpty()) {
			return groupCombinedResult;
		}
		if (combinedResult.isEmpty()) {
			combinedResult = groupCombinedResult;
		} else {
			combinedResult = combinedResult.mergeResult(groupCombinedResult);
			if (combinedResult.isEmpty()) {
				return combinedResult;
			}
		}
	}

	// Get a table of entries matching combination specified in select clause
	return this->getDesiredSynonymsResult(combinedResult);
}
