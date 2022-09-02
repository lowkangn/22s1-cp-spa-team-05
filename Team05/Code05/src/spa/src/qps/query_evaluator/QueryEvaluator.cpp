#include <qps/query_evaluator/QueryEvaluator.h>

string combine(vector<ClauseResult> results) {
	string combinedResult;
	for (ClauseResult c : results) {

	}
	return combinedResult;
};

string evaluate(Query query) {
	vector<ClauseResult> results = query.execute();
	return combine(results);
};
