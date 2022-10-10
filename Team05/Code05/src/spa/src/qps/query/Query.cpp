#include <qps/query/Query.h>

list<shared_ptr<EntityClauseResult>> Query::executeSelect(shared_ptr<PKBQueryHandler> pkb) {
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
	shared_ptr<SelectClause> firstClause = first.selectClause;
	shared_ptr<SelectClause> secondClause = second.selectClause;
	bool isSelectClauseEqual = firstClause->equals(secondClause);

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
}
