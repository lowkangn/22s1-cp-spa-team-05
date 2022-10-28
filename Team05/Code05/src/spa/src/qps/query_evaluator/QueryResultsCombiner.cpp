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
	// If not constraint results, just get cross product of select results
	if (this->resultsWithSelectedArgs.empty() && this->resultsWithoutSelectedArgs.empty()) {
		return this->getSelectSynonymsCrossProductResult();
	}

	// For the groups that have no args being selected (i.e. treat as boolean), check if empty
	vector<ClauseResult> groupCombinedResults;
	for (const vector<ClauseResult>& resultsGroup : this->resultsWithoutSelectedArgs) {
		ClauseResult groupCombinedResult = this->combineResults(resultsGroup);
		if (groupCombinedResult.isEmpty()) {
			return groupCombinedResult;
		}
		groupCombinedResults.push_back(groupCombinedResult);
	}

	// If 1) no select results, 2) no results with args being selected, 3) all group combined results with args not being selected have entries:
	// We have `Select BOOLEAN ...` with constraint clauses that can be treated as boolean and evaluate to true, so just return a table with entries.
	// Note: groupCombinedResults.front() exists since resultsWithoutSelectedArgs cannot be empty: scenario where there are no results is already dealt with in QueryEvaluator::evaluate
	if (this->selectResults.empty() && this->resultsWithSelectedArgs.empty()) {
		return groupCombinedResults.front();
	}

	// Combine clauses in groups with args being selected, then combine those groups - if any returns empty, then no query matches
	ClauseResult combinedResult;
	for (const vector<ClauseResult>& resultsGroup : this->resultsWithSelectedArgs) {
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
