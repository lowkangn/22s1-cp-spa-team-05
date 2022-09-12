#include "Query.h"

pair<shared_ptr<EntityClauseResult>, list<shared_ptr<RelationshipClauseResult>>> Query::execute() {
    shared_ptr<EntityClauseResult> entities = selectClause->execute();
    list<shared_ptr<RelationshipClauseResult>> relationships;
	list<shared_ptr<SuchThatClause>>::iterator iter = constraintClauses.begin();
	for (; iter != constraintClauses.end(); iter++) {
        relationships.push_back((*iter)->execute());
	}
	return pair(entities, relationships);
}

bool operator==(Query first, Query second) {
	bool isClauseEqual = (*(first.selectClause.get())).equals(second.selectClause.get());
	if (!isClauseEqual) {
		// different select clauses
		return false;
	} else if (first.constraintClauses.size() != second.constraintClauses.size()) {
		// different number of clauses after Select
		return false;
	}
	// check remaining clauses sequentially
	list<shared_ptr<SuchThatClause>>::iterator firstIter = first.constraintClauses.begin();
	list<shared_ptr<SuchThatClause>>::iterator secondIter = second.constraintClauses.begin();
	while (firstIter != first.constraintClauses.end()) {
		isClauseEqual = (*(*firstIter).get()).equals((*secondIter).get());
		if (!isClauseEqual) {
			return false;
		}
		firstIter++;
		secondIter++;
	}
	return true;
}
