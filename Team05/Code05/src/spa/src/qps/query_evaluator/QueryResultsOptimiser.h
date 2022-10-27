#pragma once

#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsOptimiser {
private:
	vector<ClauseResult> selectResults;
	vector<ClauseResult> relationshipsResults;
	vector<ClauseResult> withResults;

	unordered_map<ClauseArgument, vector<ClauseArgument>> adjacencyMap;
	unordered_set<ClauseArgument> visitedArgs;

	vector<vector<ClauseResult>> group(bool& isEmptyResultFound);
	void findAllConnectedArgs(const ClauseArgument& arg, vector<ClauseArgument>& currGroup);
	void sort(vector<vector<ClauseResult>>& groups);

public:
	QueryResultsOptimiser(vector<ClauseResult> selectResults, vector<ClauseResult> relationshipsResults, vector<ClauseResult> withResults) {
		this->selectResults = selectResults;
		this->relationshipsResults = relationshipsResults;
		this->withResults = withResults;
	};

	vector<vector<ClauseResult>> optimise(bool& isEmptyResultFound);
};
