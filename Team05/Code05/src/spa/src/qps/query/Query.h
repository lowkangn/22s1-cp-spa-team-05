#pragma once

#include <list>

#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/PatternClause.h>
#include <qps/query/clause/WithClause.h>
#include <pkb/PKB.h>

using namespace std;

class Query {
private:
    shared_ptr<SelectClause> selectClause;
    list<shared_ptr<RelationshipClause>> suchThatClauses;
	list<shared_ptr<PatternClause>> patternClauses;
	list<shared_ptr<WithClause>> withClauses;

    bool hasStartedExecution;
    bool emptyResultFound;

    
    template <class ClauseType>
	bool areClausesAllEqual(list<shared_ptr<ClauseType>> firstClauseList, list<shared_ptr<ClauseType>> secondClauseList) {
		if (firstClauseList.size() != secondClauseList.size()) {
			return false;
		}

		typename list<shared_ptr<ClauseType>>::iterator firstIter = firstClauseList.begin();
		typename list<shared_ptr<ClauseType>>::iterator secondIter = secondClauseList.begin();

		bool isClauseEqual;
		while (firstIter != firstClauseList.end()) {
			isClauseEqual = (*firstIter)->equals(*secondIter);
			if (!isClauseEqual) {
				return false;
			}
			firstIter++;
			secondIter++;
		}
		return true;
	}

    template <class ClauseType, class ResultType>
    void executeClauses(list<shared_ptr<ClauseType>>& clauses, list<shared_ptr<ResultType>>& results, shared_ptr<PKBQueryHandler> pkb);

    bool canEarlyStop(shared_ptr<ClauseResult> clauseResult) {
        return clauseResult->isEmpty();
    }

public:

    /* Instantiates a Query object containing the clauses. */
    Query(shared_ptr<SelectClause> select, 
        list<shared_ptr<RelationshipClause>> relationships,
        list<shared_ptr<PatternClause>> patterns, 
		list<shared_ptr<WithClause>> withs) {
        selectClause = select;
        suchThatClauses = relationships;
		patternClauses = patterns;
		withClauses = withs;
        hasStartedExecution = false;
        emptyResultFound = false;
    }

    /* Returns the results obtained from the query's SelectClause. */
    list<shared_ptr<ClauseResult>> executeSelect(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the results obtained from the query's SuchThat and Pattern clauses. */
	list<shared_ptr<RelationshipClauseResult>> executeSuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the results obtained from the query's With clauses. */
	list<shared_ptr<ClauseResult>> executeWith(shared_ptr<PKBQueryHandler> pkb);

	/* Checks if the requested return type is BOOLEAN */
	bool checkIfBooleanReturnType() {
		return selectClause->checkIfBooleanReturnType();
	}

    bool hasFoundEmptyResult() {
        return this->hasStartedExecution && this->emptyResultFound;
	}

	friend bool operator==(Query first, Query second);
};
