#pragma once

#include <qps/query/clause/ClauseResult.h>
#include <qps/query/clause/EntityClauseResult.h>

#include <algorithm>
#include <iterator>
#include <list>
#include <vector>

using namespace std;

class QueryResultsCombiner {
private:
    vector<ClauseResult> selectResults;
    vector<vector<ClauseResult>> resultsWithSelectedArgs;
    vector<vector<ClauseResult>> resultsWithoutSelectedArgs;

    ClauseResult selectBooleanPlaceholderResult =
        EntityClauseResult::createNonEmptyNoSynonymResult();

    ClauseResult mergeIntoCombinedIfNotInTable(
        ClauseResult combinedResult, ClauseResult resultToMerge) {
        if (combinedResult.isEmpty()) {
            combinedResult = resultToMerge;
        } else {
            bool isSelectArgInCombinedResult = combinedResult.checkSelectArgsInTable(
                {resultToMerge});
            if (!isSelectArgInCombinedResult) {
                combinedResult = combinedResult.mergeResult(resultToMerge);
            }
        }
        return combinedResult;
    }

    ClauseResult combineResults(const vector<ClauseResult>& results);

    ClauseResult getDesiredSynonymsResult(ClauseResult result);

    ClauseResult getSelectSynonymsCrossProductResult();

public:
    QueryResultsCombiner(
        list<ClauseResult> selectResults,
        vector<vector<vector<ClauseResult>>> optimisedConstraintResults) {
        copy(selectResults.begin(), selectResults.end(), back_inserter(this->selectResults));
        assert(optimisedConstraintResults.size() == 2);
        this->resultsWithSelectedArgs = optimisedConstraintResults.front();
        this->resultsWithoutSelectedArgs = optimisedConstraintResults.back();
    }

    vector<vector<ClauseResult>> combineWithinGroupsOnly();

    ClauseResult combineAllInternal();

    ClauseResult combineAllWithExternal(
        vector<vector<vector<ClauseResult>>>& externalOptimisedResults);
};
