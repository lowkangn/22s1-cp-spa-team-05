#include <qps/query/Query.h>

vector<ClauseResult> Query::execute() {
	std::vector<ClauseResult> results;
	std::vector<Clause>::iterator iter = clauses.begin();
	for (; iter != clauses.end(); iter++) {
		results.push_back((*iter).execute());
	}
	return results;
}
