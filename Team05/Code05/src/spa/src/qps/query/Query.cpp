#include "Query.h"

pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>> Query::execute() {
    shared_ptr<ClauseResult> entities = selectClause->execute();
    list<shared_ptr<ClauseResult>> relationships;
	list<shared_ptr<Clause>>::iterator iter = constraintClauses.begin();
	for (; iter != constraintClauses.end(); iter++) {
        relationships.push_back((*iter)->execute());
	}
	return pair(entities, relationships);
}
