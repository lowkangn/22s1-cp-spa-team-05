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
public:

    /* Instantiates a Query object containing the clauses. */
    Query(shared_ptr<SelectClause> select, 
        list<shared_ptr<RelationshipClause>> relationships,
        list<shared_ptr<PatternClause>> patterns) {
        selectClause = select;
        suchThatClauses = relationships;
		patternClauses = patterns;
    }

    /* Returns the results obtained from the query's SelectClause. */
    shared_ptr<EntityClauseResult> executeSelect(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the results obtained from the query's SuchThat and Pattern clauses. */
	list<shared_ptr<RelationshipClauseResult>> executeSuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb);

	friend bool operator==(Query first, Query second);
};
