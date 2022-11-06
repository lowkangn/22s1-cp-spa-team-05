#include <qps/query_evaluator/QueryEvaluator.h>

ClauseResult QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
    // Execute query object to obtain clauseResults
    list<ClauseResult> selectResults = dereferenceResults<ClauseResult>(query.executeSelect(pkb));
    list<ClauseResult> withResults = dereferenceResults<ClauseResult>(query.executeWith(pkb));
    list<ClauseResult> relationshipsResults = dereferenceResults<RelationshipClauseResult>(
        query.executeEarlySuchThatAndPattern(pkb));

    // If query has found empty result, short circuit and return empty result
    if (query.hasFoundEmptyResult()) {
        return EntityClauseResult::createEmptyNoSynonymResult();
    }

    // No constraints and boolean return type means the query is `Select BOOLEAN`, so return true
    if (query.checkIfBooleanReturnType() && !query.hasLateExecutionClauses() &&
        relationshipsResults.empty() && withResults.empty()) {
        return EntityClauseResult::createNonEmptyNoSynonymResult();
    }

    // Optimise
    bool isEmptyResultFound = false;
    QueryResultsOptimiser resultsOptimiser(selectResults, relationshipsResults, withResults);
    vector<vector<vector<ClauseResult>>> optimisedConstraintResults = resultsOptimiser.optimise(
        isEmptyResultFound);

    // If optimiser has found empty result, short circuit and return empty result
    if (isEmptyResultFound) {
        return EntityClauseResult::createEmptyNoSynonymResult();
    }

    // Combine
    QueryResultsCombiner combiner(selectResults, optimisedConstraintResults);
    if (!query.hasLateExecutionClauses()) {
        return combiner.combineAllInternal();
    }

    // Query has late execution clauses, optimise their execution
    vector<vector<ClauseResult>> combinedGroupedResults = combiner.combineWithinGroupsOnly();
    CfgClauseOptimiser clauseOptimiser(query, combinedGroupedResults, resultsOptimiser);
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap = clauseOptimiser.
        optimise();
    list<ClauseResult> lateResults = dereferenceResults<RelationshipClauseResult>(
        query.executeLateClauses(pkb, restrictionMap));

    if (query.hasFoundEmptyResult()) {
        return EntityClauseResult::createEmptyNoSynonymResult();
    }

    // Collate all results to be combined (ie. lateResults + previously combined results)
    for (const vector<ClauseResult>& partition : combinedGroupedResults) {
        for (const ClauseResult& groupResult : partition) {
            lateResults.emplace_back(groupResult);
        }
    }

    // Optimise the collated results
    optimisedConstraintResults = resultsOptimiser.optimise(isEmptyResultFound, lateResults);

    if (isEmptyResultFound) {
        return EntityClauseResult::createEmptyNoSynonymResult();
    }

    // Combine
    ClauseResult result = combiner.combineAllWithExternal(optimisedConstraintResults);

    return result;
}
