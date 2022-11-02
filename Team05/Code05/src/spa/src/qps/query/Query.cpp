#include <qps/query/Query.h>

list<shared_ptr<ClauseResult>> Query::executeSelect(shared_ptr<PKBQueryHandler> pkb) {
    if (this->hasFoundEmptyResult()) {
        // empty result has been found earlier, final result will be empty
        return {};
    }
    return selectClause->execute(pkb);
}

list<shared_ptr<RelationshipClauseResult>> Query::executeEarlySuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<RelationshipClauseResult>> relationships;
    this->executeClauses<PatternClause, RelationshipClauseResult>(this->patternClauses, relationships, pkb);
    this->executeClauses<RelationshipClause, RelationshipClauseResult>(this->earlySuchThatClauses, relationships, pkb);
	return relationships;
}

list<shared_ptr<ClauseResult>> Query::executeWith(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<ClauseResult>> withResults;
    this->executeClauses<WithClause, ClauseResult>(this->withClauses, withResults, pkb);
	return withResults;
}

list<shared_ptr<RelationshipClauseResult>> Query::executeLateClauses(shared_ptr<PKBQueryHandler> pkb) {
    list<shared_ptr<RelationshipClauseResult>> lateRelationships;
    if (this->hasFoundEmptyResult()) {
        //empty result was found earlier, there is no need to execute the rest
        return {};
    }
    list<shared_ptr<RelationshipClause>>::iterator clauseIter = this->lateClauses.begin();
    for (; clauseIter != this->lateClauses.end(); clauseIter++) {
        shared_ptr<RelationshipClauseResult>& result = (*clauseIter)->execute(pkb);
        lateRelationships.push_back(result);
        if (result->isEmpty()) {
            this->emptyResultFound = true;
            return {};
        }
    }
    return lateRelationships;
}

template <class ClauseType, class ResultType>
void Query::executeClauses(list<shared_ptr<ClauseType>>& clauses, list<shared_ptr<ResultType>>& results, shared_ptr<PKBQueryHandler> pkb) {
    this->hasStartedConstraintExecution = true;
    if (this->hasFoundEmptyResult()) {
        //empty result was found earlier, there is no need to execute the rest
        return;
    }

    list<shared_ptr<ClauseType>>::iterator clauseIter = clauses.begin();
    for (; clauseIter != clauses.end(); clauseIter++) {
        shared_ptr<ResultType>& result = (*clauseIter)->execute(pkb);
        results.push_back(result);
        if (result->isEmpty()) {
            this->emptyResultFound = true;
            return;
        }
    }
}


bool operator==(Query first, Query second) {
	shared_ptr<SelectClause> firstClause = first.selectClause;
	shared_ptr<SelectClause> secondClause = second.selectClause;
	bool isSelectClauseEqual = firstClause->equals(secondClause);

	if (!isSelectClauseEqual) {
		// different select clauses
		return false;
	} else if (!first.areClausesAllEqual<RelationshipClause>(first.earlySuchThatClauses, second.earlySuchThatClauses)) {
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
