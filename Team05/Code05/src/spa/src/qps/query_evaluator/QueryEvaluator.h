#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>

#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/Query.h>
#include <pkb/PKB.h>

using namespace std;

enum class KeyColumn {
	FIRST_COLUMN_KEY,
	SECOND_COLUMN_KEY
};

class QueryEvaluator {

public:
    QueryEvaluator() {};

	/* Returns the final result of a query */
	set<string> evaluate(Query query, shared_ptr<PKB> pkb);

	/* Combines the results from the clauses of a query */
    set<string> combine(shared_ptr<EntityClauseResult> entitiesResultPointer,
						list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers);

	vector<PQLEntity> filterEntities(EntityClauseResult entitiesResult,
									 list<RelationshipClauseResult> relationshipsResults);

	vector<vector<PQLEntity>> getTable(RelationshipClauseResult relationshipsResult);

	vector<vector<PQLEntity>> getKeyValueTable(RelationshipClauseResult relationshipsResult, KeyColumn keyColumn);

	int findArgumentIndex(vector<ClauseArgument> argumentsInTable, ClauseArgument argToFind);

	vector<pair<PQLEntity, vector<PQLEntity>>> convertToKeyValuePairs(vector<vector<PQLEntity>> table, int key);

	vector<pair<vector<PQLEntity>, vector<PQLEntity>>> convertToKeyValuePairs(vector<vector<PQLEntity>> table,
																			  int firstKey, int secondKey);

	vector<vector<PQLEntity>> pairKeyTableJoin(
			vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMergeKeyValuePairs);

	vector<vector<PQLEntity>> singleKeyTableJoin(
			vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMergeKeyValuePairs);

	bool combinedTableJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
						   shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable,
						   list<RelationshipClauseResult> relationshipsResults);

};
