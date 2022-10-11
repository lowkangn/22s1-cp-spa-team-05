#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/FollowsTClause.h>
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query/clause/PatternClause.h>
#include <qps/query/clause/PatternAssignClause.h>
#include <qps/query/Query.h>
#include <pkb/PKB.h>

using namespace std;

class QueryEvaluator {
private:
	list<ClauseResult> dereferenceEntityResults(list<shared_ptr<EntityClauseResult>> entityResultPointers) {
		list<ClauseResult> results;
		for (shared_ptr<EntityClauseResult> resultPointer : entityResultPointers) {
			results.push_back(*resultPointer);
		}
		return results;
	}

	list<ClauseResult> dereferenceRelationshipResults(list<shared_ptr<RelationshipClauseResult>> relationshipResultPointers) {
		list<ClauseResult> results;
		for (shared_ptr<RelationshipClauseResult> resultPointer : relationshipResultPointers) {
			results.push_back(*resultPointer);
		}
		return results;
	}

	ClauseResult combineResults(list<ClauseResult> results);

	ClauseResult getDesiredSynonymsResult(list<ClauseResult> selectResultsList, ClauseResult result);

	ClauseResult getSelectSynonymsCrossProductResult(list<ClauseResult> selectResultsList);

public:
	QueryEvaluator() {};

	set<string> evaluate(Query query, shared_ptr<PKBQueryHandler> pkb);
};
