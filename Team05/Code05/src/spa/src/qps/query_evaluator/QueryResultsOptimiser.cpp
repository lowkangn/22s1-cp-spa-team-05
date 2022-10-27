#include <qps/query_evaluator/QueryResultsOptimiser.h>

vector<vector<ClauseResult>> QueryResultsOptimiser::optimise(bool& isEmptyResultFound) {

	if (this->relationshipsResults.empty() && this->withResults.empty()) {
		return {};
	}

	vector<vector<ClauseResult>> groups = this->group(isEmptyResultFound);
	if (!isEmptyResultFound) {
		this->sort(groups);
	}
	return groups;
}

vector<vector<ClauseResult>> QueryResultsOptimiser::group(bool& isEmptyResultFound) {
	vector<ClauseResult> constraintResults;
	constraintResults.insert(constraintResults.end(), this->relationshipsResults.begin(), this->relationshipsResults.end());
	constraintResults.insert(constraintResults.end(), this->withResults.begin(), this->withResults.end());

	// We do a bit of graph DFSing: https://stackoverflow.com/questions/71359065/group-pairs-of-elements-based-on-connecting-element-in-c
	for (ClauseResult result : constraintResults) {
		if (result.isEmpty()) {
			isEmptyResultFound = true;
			return {};
		}
		vector<ClauseArgument> args = result.getSynonymArgs();
		if (args.size() == 2) {
			this->adjacencyMap[args.front()].push_back(args.back());
			this->adjacencyMap[args.back()].push_back(args.front());
		}
	}

	unordered_map<ClauseArgument, int> argToGroupIndexMap;
	int groupIndex = 0;
	for (ClauseResult result : constraintResults) {
		vector<ClauseArgument> args = result.getSynonymArgs();
		if (args.empty()) {
			continue;
		}
		if (!this->visitedArgs.count(args.front())) {
			vector<ClauseArgument> currGroup;
			this->findAllConnectedArgs(args.front(), currGroup);
			for (const ClauseArgument& argInGroup : currGroup) {
				argToGroupIndexMap[argInGroup] = groupIndex;
			}
			groupIndex++;
		}
	}

	vector<vector<ClauseResult>> groupedResults(groupIndex);
	for (ClauseResult result : constraintResults) {
		vector<ClauseArgument> args = result.getSynonymArgs();
		if (args.empty()) {
			continue;
		}
		groupedResults[argToGroupIndexMap[args.front()]].push_back(result);
	}

	return groupedResults;

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

void QueryResultsOptimiser::sort(vector<vector<ClauseResult>>& groups) {

}