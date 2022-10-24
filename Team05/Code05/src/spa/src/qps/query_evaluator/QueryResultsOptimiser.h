#pragma once

#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsOptimiser {
private:
	vector<ClauseResult> selectResults;
	vector<ClauseResult> relationshipsResults;
	vector<ClauseResult> withResults;

public:
	QueryResultsOptimiser(vector<ClauseResult> selectResults, vector<ClauseResult> relationshipsResults, vector<ClauseResult> withResults) {
		this->selectResults = selectResults;
		this->relationshipsResults = relationshipsResults;
		this->withResults = withResults;
	};

	vector<vector<ClauseResult>> optimise();
};
