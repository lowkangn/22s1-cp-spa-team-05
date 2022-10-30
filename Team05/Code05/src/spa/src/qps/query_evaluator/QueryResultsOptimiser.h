#pragma once

#include <list>
#include <qps/query/clause/ClauseResult.h>

using namespace std;

class QueryResultsOptimiser {
private:
	unordered_set<ClauseArgument> selectArgs;
	list<ClauseResult> constraintResultsList;

    /* A graph with ClauseArgs as nodes and edges connecting ClauseArgs in the same ClauseResult */
	unordered_map<ClauseArgument, vector<ClauseArgument>> adjacencyMap;

    /* A set to keep track of visited nodes during a graph traversal */
	unordered_set<ClauseArgument> visitedArgs;

    /* Divides ClauseResults in this Optimiser's results lists into 2 partitions:
     * results with selected args, and results without selected args, where each
     * partition is further divided into groups (as defined in this module)
     */
	vector<vector<vector<ClauseResult>>> group(bool& isEmptyResultFound);

    /* Performs a DFS of the ClauseArg graph to produce a group of args in DFS preordering */
    void findAllConnectedArgs(const ClauseArgument& arg, vector<ClauseArgument>& currGroup);

    /* Sorts a group of ClauseResults so that each result other than the first has
     * at least 1 preceding result with at least 1 common synonym */
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
