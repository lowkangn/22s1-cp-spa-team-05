#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
	list<ClauseResult> selectResults = dereferenceResults<ClauseResult>(query.executeSelect(pkb));
	list<ClauseResult> relationshipsResults = dereferenceResults<RelationshipClauseResult>(query.executeSuchThatAndPattern(pkb));
	list<ClauseResult> withResults = dereferenceResults<ClauseResult>(query.executeWith(pkb));

	// TODO: evaluate shouldn't be handling conversion to string
	// No results mean the query is `Select BOOLEAN`, so short circuit and return true
	if (selectResults.empty() && relationshipsResults.empty() && withResults.empty()) {
		return {"TRUE"};
	}

	// Optimise
	bool isEmptyResultFound = false;
	QueryResultsOptimiser optimiser(selectResults, relationshipsResults, withResults);
	vector<vector<vector<ClauseResult>>> optimisedConstraintResults = optimiser.optimise(isEmptyResultFound);

	// If optimiser has found empty result, short circuit and return empty result
	if (isEmptyResultFound) {
		return ClauseResult().convertTableToString(query.checkIfBooleanReturnType());
	}

	// Combine
	QueryResultsCombiner combiner(selectResults, optimisedConstraintResults);
	ClauseResult result = combiner.combine();

	// TODO: evaluate shouldn't be handling conversion to string
	return result.convertTableToString(query.checkIfBooleanReturnType());
}
