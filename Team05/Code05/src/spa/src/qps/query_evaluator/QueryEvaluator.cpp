#include <qps/query_evaluator/QueryEvaluator.h>

string combine(vector<ClauseResult> results) {
	string combinedResult;
	std::vector<ClauseResult>::iterator iter = results.begin();
	while (iter != results.end()) {

	}
	return combinedResult;
};

string evaluate(Query query) {
	vector<ClauseResult> results = query.execute();
	return combine(results);
};
