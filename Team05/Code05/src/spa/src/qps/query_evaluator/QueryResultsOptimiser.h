#pragma once

#include <qps/query/clause/ClauseResult.h>

#include <algorithm>
#include <list>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

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
    vector<vector<vector<ClauseResult>>> group(
        bool& isEmptyResultFound, list<ClauseResult>& constraintResults);

    /* Performs a DFS of the ClauseArg graph to produce a group of args in DFS preordering */
    void findAllConnectedArgs(const ClauseArgument& arg, vector<ClauseArgument>& currGroup);

    /* Sorts a group of ClauseResults so that each result other than the first has
     * at least 1 preceding result with at least 1 common synonym */
    void sort(vector<ClauseResult>& group);

public:
    QueryResultsOptimiser(
        const list<ClauseResult>& selectResults, list<ClauseResult> relationshipsResults,
        list<ClauseResult> withResults) : hasOptimised(false),
                                          argToGroupIndexMap(
                                              unordered_map<ClauseArgument, pair<int, int>>{}) {
        for (const ClauseResult& result : selectResults) {
            vector<ClauseArgument> args = result.getSynonymArgs();
            selectArgs.insert(args.begin(), args.end());
        }
        this->constraintResultsList.splice(this->constraintResultsList.end(), withResults);
        this->constraintResultsList.splice(
            this->constraintResultsList.end(),
            relationshipsResults);
    }

    vector<vector<vector<ClauseResult>>> optimise(bool& isEmptyResultFound);

    vector<vector<vector<ClauseResult>>> optimise(
        bool& isEmptyResultFound, list<ClauseResult>& constraintResults);

    optional<pair<int, int>> getArgGroupIndex(ClauseArgument arg) {
        if (!hasOptimised) {
            throw PQLLogicError("Optimisation must be done before querying for arg group");
        }
        if (this->argToGroupIndexMap.count(arg) <= 0) {
            return optional<pair<int, int>>();
        }

        return optional<pair<int, int>>(this->argToGroupIndexMap.at(arg));
    }
};
