#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
	list<ClauseResult> entitiesResults = dereferenceResults<ClauseResult>(query.executeSelect(pkb));
	list<ClauseResult> relationshipsResults = dereferenceResults<RelationshipClauseResult>(query.executeSuchThatAndPattern(pkb));
	list<ClauseResult> withResults = dereferenceResults<ClauseResult>(query.executeWith(pkb));

	// Empty lists indicate `Select BOOLEAN` with no constraint clauses, in which case just return TRUE
	if (entitiesResults.empty() && relationshipsResults.empty() && withResults.empty()) {
		return set<string>{"TRUE"};
	}

	if (relationshipsResults.empty() && withResults.empty()) {
		ClauseResult selectSynonymsCrossProductResult = this->getSelectSynonymsCrossProductResult(entitiesResults);
		return selectSynonymsCrossProductResult.convertTableToString(false);
	}

	// Combine constraint clauses - if returns empty, then no query matches
	withResults.splice(withResults.end(), relationshipsResults);
	ClauseResult combinedResult = this->combineResults(withResults);
	if (combinedResult.isEmpty()) {
		return combinedResult.convertTableToString(query.checkIfBooleanReturnType());
	}

	// Get a table of entries matching combination specified in select clause
	ClauseResult desiredSynonymsResult = this->getDesiredSynonymsResult(entitiesResults, combinedResult);

	// Convert results to strings
	return desiredSynonymsResult.convertTableToString(query.checkIfBooleanReturnType());
}

ClauseResult QueryEvaluator::combineResults(list<ClauseResult> results) {
	// TODO: Do a little optimisation here x3

	// Iterate and combine results
	ClauseResult combinedResult;
	for (list<ClauseResult>::iterator resultIter = results.begin(); resultIter != results.end(); resultIter++) {
		if (resultIter == results.begin()) {
			combinedResult = *resultIter;
		} else {
			ClauseResult newCombinedResult = combinedResult.mergeResult(*resultIter);
			combinedResult = newCombinedResult;
		}
	}

	return combinedResult;
}

ClauseResult QueryEvaluator::getDesiredSynonymsResult(list<ClauseResult> selectResultsList, ClauseResult combinedResult) {
	// If no select results, we are returning boolean, so just return table
	if (selectResultsList.empty()) {
		return combinedResult;
	}

	// Convert to vector
	vector<ClauseResult> selectResults;
	selectResults.insert(selectResults.end(), selectResultsList.begin(), selectResultsList.end());

	// Check if any select args in combined table - if not, do cross product with select results
	bool areSelectArgsInCombinedTable = combinedResult.checkSelectArgsInTable(selectResults);
	if (!areSelectArgsInCombinedTable) {
		return this->getSelectSynonymsCrossProductResult(selectResultsList);
	} else {
		return combinedResult.rearrangeTableToMatchSelectResults(selectResults);
	}
}

ClauseResult QueryEvaluator::getSelectSynonymsCrossProductResult(list<ClauseResult> selectResultsList) {
	// Convert to vector
	vector<ClauseResult> selectResults;
	selectResults.insert(selectResults.end(), selectResultsList.begin(), selectResultsList.end());
	ClauseResult combinedResult;

	vector<ClauseResult> selectResultsWithoutAttribute;

	for (ClauseResult selectResult : selectResults) {
		combinedResult = mergeIntoCombinedIfNotInTable(combinedResult, selectResult);
	}

	return this->getDesiredSynonymsResult(selectResultsList, combinedResult);
}
