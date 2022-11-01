#include <qps/query/Query.h>

list<shared_ptr<ClauseResult>> Query::executeSelect(shared_ptr<PKBQueryHandler> pkb) {
    return selectClause->execute(pkb);
}

list<shared_ptr<RelationshipClauseResult>> Query::executeSuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<RelationshipClauseResult>> relationships;
    this->executeClauses<PatternClause, RelationshipClauseResult>(this->patternClauses, relationships, pkb);
    this->executeClauses<RelationshipClause, RelationshipClauseResult>(this->suchThatClauses, relationships, pkb);
	return relationships;
}

list<shared_ptr<ClauseResult>> Query::executeWith(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<ClauseResult>> withResults;
    this->executeClauses<WithClause, ClauseResult>(this->withClauses, withResults, pkb);
	return withResults;
}

template <class ClauseType, class ResultType>
void Query::executeClauses(list<shared_ptr<ClauseType>>& clauses, list<shared_ptr<ResultType>>& results, shared_ptr<PKBQueryHandler> pkb) {
    list<shared_ptr<ClauseType>>::iterator clauseIter = clauses.begin();

    for (; clauseIter != clauses.end(); clauseIter++) {
        shared_ptr<ResultType> result = (*clauseIter)->execute(pkb);
        results.push_back(result);
        //if (canEarlyStop(result)) {
          //  return;
        //}
    }
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
	} else if (!first.areClausesAllEqual<WithClause>(first.withClauses, second.withClauses)) {
		// different with clauses
		return false;
	}

	return true;
}
