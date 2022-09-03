#pragma once

#include <list>

#include <qps/query_parser/Declaration.h>
#include <qps/query/clause/ClauseResult.h>
#include <qps/query/clause/Clause.h>

using namespace std;

class Query {
private:
	Clause selectClause;
	list<Clause> constraintClauses;
public:
	
	/* Instantiates a Query object containign the clauses. */
	Query(Clause select, list<Clause> constraints) {
		selectClause = select;
		constraintClauses = constraints;
	}

	/* Returns the results obtained from each of this query's clauses. */
	list<ClauseResult> Query::execute();
};