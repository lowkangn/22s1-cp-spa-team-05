#pragma once

#include <list>
#include <string>
#include <unordered_set>

using namespace std;

/* QueryResultsProjector is responsible for 
	returning results in the manner required by 
	the autotester. */
class QueryResultsProjector {
private:
	unordered_set<string> evaluatorResults;
public:
	QueryResultsProjector(unordered_set<string>& evaluatorResults) 
		: evaluatorResults(evaluatorResults) {};

	void populateResultsList(list<string>& autotesterResults) {
		for (string result : this->evaluatorResults) {
			autotesterResults.emplace_back(result);
		}
	}
};