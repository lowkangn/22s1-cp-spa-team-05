#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
	vector<ClauseResult> selectResults = dereferenceResults<ClauseResult>(query.executeSelect(pkb));
	vector<ClauseResult> relationshipsResults = dereferenceResults<RelationshipClauseResult>(query.executeSuchThatAndPattern(pkb));
	vector<ClauseResult> withResults = dereferenceResults<ClauseResult>(query.executeWith(pkb));

	// TODO: evaluate shouldn't be handling conversion to string
	if (selectResults.empty() && relationshipsResults.empty() && withResults.empty()) {
		return {"TRUE"};
	}

	QueryResultsOptimiser optimiser = QueryResultsOptimiser();
	vector<vector<ClauseResult>> optimisedConstraintResults = optimiser.optimise(selectResults, relationshipsResults, withResults);

	QueryResultsCombiner combiner = QueryResultsCombiner();
	ClauseResult result = combiner.combine(selectResults, optimisedConstraintResults);
	// TODO: evaluate shouldn't be handling conversion to string
	return result.convertTableToString(query.checkIfBooleanReturnType());
}
