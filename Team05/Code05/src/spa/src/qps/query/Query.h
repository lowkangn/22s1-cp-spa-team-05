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
    
    template <class T>
	bool areClausesAllEqual(list<shared_ptr<T>> firstClauseList, list<shared_ptr<T>> secondClauseList) {
		if (firstClauseList.size() != secondClauseList.size()) {
			return false;
		}

		typename list<shared_ptr<T>>::iterator firstIter = firstClauseList.begin();
		typename list<shared_ptr<T>>::iterator secondIter = secondClauseList.begin();

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
    }

    /* Returns the results obtained from the query's SelectClause. */
    list<shared_ptr<EntityClauseResult>> executeSelect(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the results obtained from the query's SuchThat and Pattern clauses. */
	list<shared_ptr<RelationshipClauseResult>> executeSuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the results obtained from the query's With clauses. */
	list<shared_ptr<ClauseResult>> executeWith(shared_ptr<PKBQueryHandler> pkb);

	/* Checks if the requested return type is BOOLEAN */
	bool checkIfBooleanReturnType() {
		return selectClause->checkIfBooleanReturnType();
	}

	friend bool operator==(Query first, Query second);
};
