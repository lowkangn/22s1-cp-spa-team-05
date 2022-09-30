#include <qps/query/Query.h>

shared_ptr<EntityClauseResult> Query::executeSelect(shared_ptr<PKBQueryHandler> pkb) {
    return selectClause->execute(pkb);
}

list<shared_ptr<RelationshipClauseResult>> Query::executeSuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<RelationshipClauseResult>> relationships;
	list<shared_ptr<RelationshipClause>>::iterator suchThatIter = suchThatClauses.begin();
	for (; suchThatIter != suchThatClauses.end(); suchThatIter++) {
		relationships.push_back((*suchThatIter)->execute(pkb));
	}
	list<shared_ptr<PatternClause>>::iterator patternIter = patternClauses.begin();
	for (; patternIter != patternClauses.end(); patternIter++) {
		relationships.push_back((*patternIter)->execute(pkb));
	}
	return relationships;
}

bool operator==(Query first, Query second) {
	bool isSelectClauseEqual = (*(first.selectClause.get())).equals(second.selectClause.get());

	//TODO: Refactor similar logic into private generic helper method e.g. checkListsEqual<T>( list<shared_ptr<T>> a, list<shared_ptr<T>> b)

	if (!isSelectClauseEqual) {
		// different select clauses
		return false;
	} else if (!first.areClausesAllEqual<RelationshipClause>(first.suchThatClauses, second.suchThatClauses)) {
		// different such that clauses
		return false;
	} else if (!first.areClausesAllEqual<PatternClause>(first.patternClauses, second.patternClauses)) {
		// different pattern clauses
		return false;
	}
	return true;
	/*
	else if (first.suchThatClauses.size() != second.suchThatClauses.size()) {
		// different number of such that clauses after Select
		return false;
	}
	else if (first.patternClauses.size() != second.patternClauses.size()) {
		// different number of pattern clauses after Select
		return false;
	}
	
	// check remaining clauses sequentially
	list<shared_ptr<RelationshipClause>>::iterator firstIter = first.suchThatClauses.begin();
	list<shared_ptr<RelationshipClause>>::iterator secondIter = second.suchThatClauses.begin();
	while (firstIter != first.suchThatClauses.end()) {
		isClauseEqual = (*firstIter)->equals(*secondIter);
		if (!isClauseEqual) {
			return false;
		}
		firstIter++;
		secondIter++;
	}

	list<shared_ptr<PatternClause>>::iterator firstPatternIter = first.patternClauses.begin();
	list<shared_ptr<PatternClause>>::iterator secondPatternIter = second.patternClauses.begin();
	while (firstPatternIter != first.patternClauses.end()) {
		isClauseEqual = (*firstPatternIter)->equals(*secondPatternIter);
		if (!isClauseEqual) {
			return false;
		}
		firstPatternIter++;
		secondPatternIter++;
	}
	
	return true;*/
	
}
