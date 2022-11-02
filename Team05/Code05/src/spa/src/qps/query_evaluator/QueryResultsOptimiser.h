#pragma once

#include <list>
#include <qps/query/clause/ClauseResult.h>

using namespace std;

inline int SELECT_PARTITION_INDEX = 0;
inline int NONSELECT_PARTITION_INDEX = 1;

class QueryResultsOptimiser {
private:
	unordered_set<ClauseArgument> selectArgs;
	list<ClauseResult> constraintResultsList;
    bool hasOptimised;

    unordered_map<ClauseArgument, pair<int, int>> argToGroupIndexMap;

    /* A graph with ClauseArgs as nodes and edges connecting ClauseArgs in the same ClauseResult */
	unordered_map<ClauseArgument, vector<ClauseArgument>> adjacencyMap;

    /* A set to keep track of visited nodes during a graph traversal */
	unordered_set<ClauseArgument> visitedArgs;

    /* Divides ClauseResults in constraintResults into 2 partitions:
     * results with selected args, and results without selected args, where each
     * partition is further divided into groups (as defined in this module)
     */
    vector<vector<vector<ClauseResult>>> group(bool& isEmptyResultFound,
        list<ClauseResult>& constraintResults);

    /* Performs a DFS of the ClauseArg graph to produce a group of args in DFS preordering */
    void findAllConnectedArgs(const ClauseArgument& arg, vector<ClauseArgument>& currGroup);

    /* Sorts a group of ClauseResults so that each result other than the first has
     * at least 1 preceding result with at least 1 common synonym */
    void sort(vector<ClauseResult>& group);

public:
	QueryResultsOptimiser(const list<ClauseResult>& selectResults, list<ClauseResult> relationshipsResults, list<ClauseResult> withResults)
        : argToGroupIndexMap(unordered_map<ClauseArgument, pair<int,int>>{}), hasOptimised(false) {
		for (ClauseResult result : selectResults) {
			vector<ClauseArgument> args = result.getSynonymArgs();
			selectArgs.insert(args.begin(), args.end());
		}
        this->constraintResultsList.splice(this->constraintResultsList.end(), withResults);
		this->constraintResultsList.splice(this->constraintResultsList.end(), relationshipsResults);
	}

    vector<vector<vector<ClauseResult>>> optimise(bool& isEmptyResultFound);

	vector<vector<vector<ClauseResult>>> optimise(bool& isEmptyResultFound, list<ClauseResult>& constraintResults);

    unordered_map<ClauseArgument, pair<int, int>>& getArgToGroupIndexMap() {
        if (!hasOptimised) {
            bool placeholder = false;
            this->optimise(placeholder, this->constraintResultsList);
        }
        return this->argToGroupIndexMap;
    }
};
