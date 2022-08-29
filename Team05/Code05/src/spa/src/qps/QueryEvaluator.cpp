#include <QueryEvaluator.h>

vector<string> combine(vector<ClauseResult> results) {
	vector<string> combinedResult;
	std::vector<ClauseResult>::iterator iter = results.begin();
	while (iter != results.end()) {

	}
	return combinedResult;
};

vector<string> evaluate(Query query) {
	vector<ClauseResult> results = query.execute();
	return combine(results);
};
