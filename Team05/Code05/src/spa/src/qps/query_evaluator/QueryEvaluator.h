#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/Query.h>
#include <pkb/PKB.h>

using namespace std;

enum class RelationshipArgument {
    ARG1,
    ARG2,
    NONE
};

class QueryEvaluator {
public:
    QueryEvaluator() {};

	/* Returns the final result of a query */
	set<string> evaluate(Query query, shared_ptr<PKBQueryHandler> pkb);

	/* Combines the results from the clauses of a query */
    set<string> combine(shared_ptr<EntityClauseResult> entitiesResultPointer,
						list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers);

	RelationshipArgument findDesiredArgument(ClauseArgument desiredArg,
											 RelationshipClauseResult relationshipResultToCheck);

	list<RelationshipClauseResult> dereferenceRelationshipsResultPointers(
			list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers);

	set<PQLEntity> extractEntitySet(RelationshipArgument argToExtract, vector<PQLRelationship> relationships);

	set<PQLEntity> intersectSets(set<PQLEntity> firstSet, set<PQLEntity> secondSet);

};
