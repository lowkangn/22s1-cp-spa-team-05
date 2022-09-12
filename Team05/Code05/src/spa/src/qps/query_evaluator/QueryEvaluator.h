#pragma once

#include <string>
#include <vector>
#include <set>

#include "../query/clause/ClauseResult.h"
#include "../query/clause/EntityClauseResult.h"
#include "../query/clause/RelationshipClauseResult.h"
#include "../query/Query.h"

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
	set<string> evaluate(Query query);

	/* Combines the results from the clauses of a query */
    set<string> combine(shared_ptr<ClauseResult> entitiesResultPointer,
						list<shared_ptr<ClauseResult>> relationshipsResultPointers);

	RelationshipArgument findDesiredArgument(ClauseArgument desiredArg,
											 RelationshipClauseResult relationshipResultToCheck);

	EntityClauseResult dereferenceEntitiesResultPointer(shared_ptr<ClauseResult> entitiesResultPointer);

	list<RelationshipClauseResult> dereferenceRelationshipsResultPointers(
			list<shared_ptr<ClauseResult>> relationshipsResultPointers);

	set<PQLEntity> extractEntitySet(RelationshipArgument argToExtract, vector<PQLRelationship> relationships);

	set<PQLEntity> intersectSets(set<PQLEntity> firstSet, set<PQLEntity> secondSet);

};
