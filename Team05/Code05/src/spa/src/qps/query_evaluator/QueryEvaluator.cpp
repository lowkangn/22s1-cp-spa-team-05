#include <qps/query_evaluator/QueryEvaluator.h>

ClauseResult QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
    // Execute query object to obtain clauseResults
	list<ClauseResult> selectResults = dereferenceResults<ClauseResult>(query.executeSelect(pkb));
    list<ClauseResult> withResults = dereferenceResults<ClauseResult>(query.executeWith(pkb));
    list<ClauseResult> relationshipsResults = dereferenceResults<RelationshipClauseResult>(query.executeEarlySuchThatAndPattern(pkb));

    // If query has found empty result, short circuit and return empty result
    if (query.hasFoundEmptyResult()) {
        return EntityClauseResult::createEmptyNoSynonymResult();
    }

    if (query.hasLateExecutionClauses()) {
        list<ClauseResult> newResults = dereferenceResults<RelationshipClauseResult>(query.executeLateClauses(pkb));
        relationshipsResults.splice(relationshipsResults.end(), newResults);
    }


	// No constraints and boolean return type means the query is `Select BOOLEAN`, so return true
	if (query.checkIfBooleanReturnType() && !query.hasLateExecutionClauses() && relationshipsResults.empty() && withResults.empty()) {
        return EntityClauseResult::createNonEmptyNoSynonymResult();
	}

	// Optimise
    bool isEmptyResultFound = false;
    QueryResultsOptimiser optimiser(selectResults, relationshipsResults, withResults);
	vector<vector<vector<ClauseResult>>> optimisedConstraintResults = optimiser.optimise(isEmptyResultFound);

	// If optimiser has found empty result, short circuit and return empty result
	if (isEmptyResultFound) {
		return EntityClauseResult::createEmptyNoSynonymResult();
	}

	// Combine
	QueryResultsCombiner combiner(selectResults, optimisedConstraintResults);
	ClauseResult result = combiner.combineAll();
    
	return result;
}
