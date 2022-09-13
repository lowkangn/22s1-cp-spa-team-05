#pragma once

#include <list>

#include "qps/query/clause/SelectClause.h"
#include "qps/query/clause/SuchThatClause.h"

using namespace std;

class Query {
private:
    shared_ptr<SelectClause> selectClause;
    list<shared_ptr<SuchThatClause>> constraintClauses;
public:

    /* Instantiates a Query object containing the clauses. */
    Query(shared_ptr<SelectClause> select, list<shared_ptr<SuchThatClause>> constraints) {
        selectClause = select;
        constraintClauses = constraints;
    }

    /* Returns the results obtained from the query's SelectClause. */
    shared_ptr<EntityClauseResult> executeSelect();

	/* Returns the results obtained from the query's SuchThat clauses. */
	list<shared_ptr<RelationshipClauseResult>> executeSuchThat();

	friend bool operator==(Query first, Query second);
};
