#pragma once

#include <qps/query/clause/ClauseResult.h>

class QueryResultsOptimiser {
public:
	QueryResultsOptimiser() {};

	vector<vector<ClauseResult>> optimise(vector<ClauseResult> selectResults, vector<ClauseResult> relationshipResults, vector<ClauseResult> withResults);
};
