#pragma once

#include <qps/query/clause/EntityClauseResult.h>

#include <list>
#include <set>
#include <string>

using namespace std;

/* QueryResultsProjector is responsible for 
	returning results in the manner required by 
	the autotester. */
class QueryResultsProjector {
private:
    ClauseResult& evaluatorResult;
public:
    explicit QueryResultsProjector(ClauseResult& evaluatorResult)
        : evaluatorResult(evaluatorResult) {}

    void populateResultsList(
        list<string>& autotesterResults, bool isReturningBoolean, string errorString);
};
