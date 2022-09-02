#pragma once

#include <string>
#include <vector>

#include <qps/query/clause/ClauseResult.h>
#include <qps/query/Query.h>

using namespace std;

class QueryEvaluator {
public:
	/* Returns the final result of a query */
	string evaluate(Query query);

	/* Combines the results from the clauses of a query */
	string combine(vector<ClauseResult>);

};