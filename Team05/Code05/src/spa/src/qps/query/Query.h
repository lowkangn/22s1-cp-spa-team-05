#pragma once

#include <list>

#include <qps/query/clause/ClauseResult.h>
#include <qps/query/clause/Clause.h>

using namespace std;

class Query {
private:
	shared_ptr<Clause> selectClause;
	list<shared_ptr<Clause>> constraintClauses;
public:

	/* Instantiates a Query object containing the clauses. */
	Query(shared_ptr<Clause> select, list<shared_ptr<Clause>> constraints) {
		selectClause = select;
		constraintClauses = constraints;
	}

	/* Returns the results obtained from each of this query's clauses. */
	list<ClauseResult> execute();

	friend bool operator==(Query first, Query second);
};