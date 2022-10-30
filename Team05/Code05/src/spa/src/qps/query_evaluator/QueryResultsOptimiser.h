#pragma once

#include <list>
#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsOptimiser {
private:
	unordered_set<ClauseArgument> selectArgs;
	list<ClauseResult> constraintResultsList;

	unordered_map<ClauseArgument, vector<ClauseArgument>> adjacencyMap;
	unordered_set<ClauseArgument> visitedArgs;

	vector<vector<vector<ClauseResult>>> group(bool& isEmptyResultFound);
	void findAllConnectedArgs(const ClauseArgument& arg, vector<ClauseArgument>& currGroup);
	void sort(vector<ClauseResult>& group);

public:
	QueryResultsOptimiser(const list<ClauseResult>& selectResults, list<ClauseResult> relationshipsResults, list<ClauseResult> withResults) {
		for (ClauseResult result : selectResults) {
			vector<ClauseArgument> args = result.getSynonymArgs();
			selectArgs.insert(args.begin(), args.end());
		}
		this->constraintResultsList.splice(this->constraintResultsList.end(), relationshipsResults);
		this->constraintResultsList.splice(this->constraintResultsList.end(), withResults);
	};

	vector<vector<vector<ClauseResult>>> optimise(bool& isEmptyResultFound);
};
