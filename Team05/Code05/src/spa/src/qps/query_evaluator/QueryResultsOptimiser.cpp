#include <qps/query_evaluator/QueryResultsOptimiser.h>

vector<vector<vector<ClauseResult>>> QueryResultsOptimiser::optimise(bool& isEmptyResultFound) {
    return this->optimise(isEmptyResultFound, this->constraintResultsList);
}

vector<vector<vector<ClauseResult>>> QueryResultsOptimiser::optimise(bool& isEmptyResultFound,
    list<ClauseResult>& constraintResults) {
    this->hasOptimised = true;
    if (constraintResults.empty()) {
        return { {}, {} };
    }

    vector<vector<vector<ClauseResult>>> groups = this->group(isEmptyResultFound, constraintResults);
    if (!isEmptyResultFound) {
        this->visitedArgs.clear();
        for (vector<ClauseResult>& group : groups.front()) {
            this->sort(group);
        }
        for (vector<ClauseResult>& group : groups.back()) {
            this->sort(group);
        }
    }
    return groups;
}

vector<vector<vector<ClauseResult>>> QueryResultsOptimiser::group(bool& isEmptyResultFound,
    list<ClauseResult>& constraintResults) {

	// We do a bit of graph DFSing: https://stackoverflow.com/questions/71359065/group-pairs-of-elements-based-on-connecting-element-in-c
	// Populate adjacency map
    for (const ClauseResult& result : constraintResults) {
        if (result.isEmpty()) {
            isEmptyResultFound = true;
            return { {}, {} };
        }
        vector<ClauseArgument> args = result.getSynonymArgs();
        if (args.size() == 2) {
            this->adjacencyMap[args.front()].push_back(args.back());
            this->adjacencyMap[args.back()].push_back(args.front());
        }
    }

	// For each arg not yet visited, traverse graph for all connected args and assign the same group index to all of them
    int selectedArgGroupIndex = 0;
    int nonSelectedArgGroupIndex = 0;
    this->visitedArgs.clear();

	for (const ClauseResult& result : constraintResults) {
		vector<ClauseArgument> args = result.getSynonymArgs();
		if (args.empty()) {
			continue;
		}
		if (!this->visitedArgs.count(args.front())) {
            vector<ClauseArgument> currGroup;
            this->findAllConnectedArgs(args.front(), currGroup);

            int partitionIndex;
            int partitionGroupIndex;
            bool groupHasSelectArg = false;
            for (const ClauseArgument& argInGroup : currGroup) {
                groupHasSelectArg = groupHasSelectArg || this->selectArgs.count(argInGroup);
            }
            if (groupHasSelectArg) {
                partitionIndex = SELECT_PARTITION_INDEX;
                partitionGroupIndex = selectedArgGroupIndex++;
            } else {
                partitionIndex = NONSELECT_PARTITION_INDEX;
                partitionGroupIndex = nonSelectedArgGroupIndex++;
            }

            for (const ClauseArgument& argInGroup : currGroup) {
                this->argToGroupIndexMap[argInGroup] = pair<int, int>(partitionIndex, partitionGroupIndex);
            }
		}
	}

	// divide results into groups based on group index of each result's first arg
    vector<vector<ClauseResult>> groupsWithSelectArgs(selectedArgGroupIndex);
    vector<vector<ClauseResult>> groupsWithoutSelectArgs(nonSelectedArgGroupIndex);
    vector<vector<vector<ClauseResult>>> out = { groupsWithSelectArgs, groupsWithoutSelectArgs };
    for (const ClauseResult & result : constraintResults) {
        vector<ClauseArgument> args = result.getSynonymArgs();
        if (args.empty()) {
            continue;
        }
        pair<int,int> currentGroupIndex = this->argToGroupIndexMap[args.front()];
        out[currentGroupIndex.first][currentGroupIndex.second].emplace_back(result);
    }

	return out;
}

void QueryResultsOptimiser::findAllConnectedArgs(const ClauseArgument& arg, vector<ClauseArgument>& currGroup) {
	if (this->visitedArgs.count(arg)) {
		return;
	}
	this->visitedArgs.insert(arg);
	currGroup.push_back(arg);
	for (const ClauseArgument& adjacentArg : this->adjacencyMap[arg]) {
		this->findAllConnectedArgs(adjacentArg, currGroup);
	}
}

void QueryResultsOptimiser::sort(vector<ClauseResult>& group) {
    // if a group has 2 or fewer results, it already has no cross joins, so return
    // note after this step, all groups will have at least 1 synonym
    if (group.size() <= 2) {
        return;
    }

    // Construct map to retrieve results corresponding to an argument
    unordered_map<ClauseArgument, vector<ClauseResult>> argToResultMap;
    for (ClauseResult& result : group) {
        vector<ClauseArgument> args = result.getSynonymArgs();
        for (const ClauseArgument& arg : args) {
            argToResultMap[arg].emplace_back(result);
        }
    }

    // Traverse the argument map to find a DFS preordering of ClauseArgs
    // Note: we use the first synonym arg of the first ClauseResult as the starting node
    // It may be possible to further optimise by using a 'better' starting node
    vector<ClauseArgument> orderingInGroup;
    const ClauseArgument arg = group.front().getSynonymArgs().front();
    this->findAllConnectedArgs(arg, orderingInGroup);

    // Retrieve ClauseResults corresponding to the order obtained
    set<ClauseResult> addedResult;
    group.clear();
    for (ClauseArgument& arg : orderingInGroup) {
        for (ClauseResult& result: argToResultMap[arg]) {
            if (addedResult.count(result)) {
                continue;
            }
            addedResult.insert(result);
            group.emplace_back(result);
        }
    }
}