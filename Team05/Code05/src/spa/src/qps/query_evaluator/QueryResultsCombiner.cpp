#include <qps/query_evaluator/QueryResultsCombiner.h>

ClauseResult QueryResultsCombiner::combineResults(const vector<ClauseResult>& results) {
	// Iterate and combine results
	ClauseResult combinedResult;
	for (vector<ClauseResult>::const_iterator resultIter = results.begin(); resultIter != results.end(); ++resultIter) {
		if (resultIter == results.begin()) {
			combinedResult = *resultIter;
		} else {
			ClauseResult newCombinedResult = combinedResult.mergeResult(*resultIter);
			combinedResult = newCombinedResult;
		}
	}

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
		combinedResult = this->mergeIntoCombinedIfNotInTable(combinedResult, selectResult);
	}

	return this->getDesiredSynonymsResult(combinedResult);
}

vector<vector<ClauseResult>> QueryResultsCombiner::combineWithinGroupsOnly() {
    vector<vector<ClauseResult>> out = { {}, {} };
    for (const vector<ClauseResult>& resultsGroup : this->resultsWithoutSelectedArgs) {
        out.front().emplace_back(this->combineResults(resultsGroup));
    }
    for (const vector<ClauseResult>& resultsGroup : this->resultsWithSelectedArgs) {
        out.back().emplace_back(this->combineResults(resultsGroup));
    }
    return out;
}

ClauseResult QueryResultsCombiner::combineAll() {
	// If no select results and no results without selected args, can treat as `Select BOOLEAN` so just return true
	if (this->selectResults.empty() && this->resultsWithoutSelectedArgs.empty()) {
		assert(this->resultsWithSelectedArgs.empty()); // For select BOOLEAN, no args are being selected
		return this->selectBooleanPlaceholderResult;
	}

	// If no constraint results, just get cross product of select results
	if (this->resultsWithSelectedArgs.empty() && this->resultsWithoutSelectedArgs.empty()) {
		return this->getSelectSynonymsCrossProductResult();
	}

	// For the groups that have no args being selected (i.e. treat as boolean), check if empty
	for (const vector<ClauseResult>& resultsGroup : this->resultsWithoutSelectedArgs) {
		ClauseResult groupCombinedResult = this->combineResults(resultsGroup);
		if (groupCombinedResult.isEmpty()) {
			return {};
		}
	}

	// Since constraint groups with no select args are non-empty, they all evaluate to true.
	// So if there are no select results, we have `Select BOOLEAN ...(true)` --> return true.
	if (this->selectResults.empty()) {
		return this->selectBooleanPlaceholderResult;
	}

	// Combine clauses in groups with args being selected, then combine those groups
	// - if any returns empty, then no query matches
	ClauseResult combinedResult;
	for (const vector<ClauseResult>& resultsGroup : this->resultsWithSelectedArgs) {
		ClauseResult groupCombinedResult = this->combineResults(resultsGroup);
		if (groupCombinedResult.isEmpty()) {
			return {};
		}
		if (combinedResult.isEmpty()) {
			combinedResult = groupCombinedResult;
		} else {
			combinedResult = combinedResult.mergeResult(groupCombinedResult);
			if (combinedResult.isEmpty()) {
				return {};
			}
		}
	}

	// Get a table of entries matching combination specified in select clause
	return this->getDesiredSynonymsResult(combinedResult);
}
