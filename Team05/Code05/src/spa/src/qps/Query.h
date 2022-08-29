#pragma once

#include <vector>

#include <ClauseResult.h>
#include <Clause.h>

using std::vector;

class Query {
private:
	vector<Clause> clauses;
public:
	
	/* Instantiates a Query object containign the clauses. */
	Query(vector<Clause> clauses) {
		this -> clauses = clauses;
	}

	/* Returns the results obtained from each of this query's clauses. */
	vector<ClauseResult> Query::execute();
};