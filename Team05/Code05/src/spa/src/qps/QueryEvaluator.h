#pragma once

#include <string>
#include <vector>

#include <ClauseResult.h>
#include <Query.h>

using std::vector;
using std::string;

class QueryEvaluator {
public:
	/* Returns the final result of a query */
	vector<string> evaluate(Query query);

	/* Combines the results from the clauses of a query */
	vector<string> combine(vector<ClauseResult>);

};