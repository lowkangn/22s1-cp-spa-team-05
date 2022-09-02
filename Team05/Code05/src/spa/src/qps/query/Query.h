#pragma once

#include <vector>

#include <qps/query/clause/ClauseResult.h>
#include <qps/query/clause/Clause.h>

using namespace std;

class Query {
private:
	vector<Clause> clauses;
public:
	
	/* Instantiates a Query object containing the clauses. */
	Query(vector<Clause> clauses) {
		this -> clauses = clauses;
	}

	/* Returns the results obtained from each of this query's clauses. */
	vector<ClauseResult> Query::execute();
};