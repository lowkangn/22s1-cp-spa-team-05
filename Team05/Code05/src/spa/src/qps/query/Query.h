#pragma once

#include <list>

#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/RelationshipClause.h>
#include <pkb/PKB.h>

using namespace std;

class Query {
private:
    shared_ptr<SelectClause> selectClause;
    list<shared_ptr<RelationshipClause>> constraintClauses;
public:

    /* Instantiates a Query object containing the clauses. */
    Query(shared_ptr<SelectClause> select, list<shared_ptr<RelationshipClause>> constraints) {
        selectClause = select;
        constraintClauses = constraints;
    }

    /* Returns the results obtained from the query's SelectClause. */
    shared_ptr<EntityClauseResult> executeSelect(shared_ptr<PKB> pkb);

	/* Returns the results obtained from the query's SuchThat clauses. */
	list<shared_ptr<RelationshipClauseResult>> executeSuchThat(shared_ptr<PKB> pkb);

	friend bool operator==(Query first, Query second);
};
