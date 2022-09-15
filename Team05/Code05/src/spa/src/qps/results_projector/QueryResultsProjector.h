#pragma once

#include <list>
#include <string>
#include <set>

using namespace std;

/* QueryResultsProjector is responsible for 
	returning results in the manner required by 
	the autotester. */
class QueryResultsProjector {
private:
	set<string> evaluatorResults;
public:
	QueryResultsProjector(set<string>& evaluatorResults)
		: evaluatorResults(evaluatorResults) {};

	void populateResultsList(list<string>& autotesterResults) {
		for (string result : this->evaluatorResults) {
			autotesterResults.emplace_back(result);
		}
	}
};