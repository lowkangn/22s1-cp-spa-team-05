#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

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

private:
	bool createNewTable(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
						shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable,
						shared_ptr<list<RelationshipClauseResult>> resultsSkipped,
						RelationshipClauseResult relationshipsResult) {

		if (combinedTable->empty()) {
			*combinedTable = this->getTable(relationshipsResult);
			argumentsInCombinedTable->push_back(relationshipsResult.getFirstArg());
			argumentsInCombinedTable->push_back(relationshipsResult.getSecondArg());
			return true;
		}

		// If equals -1, means ClauseArguments not yet in combined table
		int firstArgIndex = this->findArgumentIndex(*argumentsInCombinedTable, relationshipsResult.getFirstArg());
		int secondArgIndex = this->findArgumentIndex(*argumentsInCombinedTable, relationshipsResult.getSecondArg());

		// Neither clause argument already in currentTable so cannot join, so skip
		if (firstArgIndex == -1 && secondArgIndex == -1) {
			resultsSkipped->push_back(relationshipsResult);
			return true;
		}

		// Create new table
		vector<vector<PQLEntity>> newCombinedTable;

		// Both clause arguments already in table, table join on these common clause arguments
		if (firstArgIndex != -1 && secondArgIndex != -1) {
			newCombinedTable = this->makeKeyValuePairsAndJoin(combinedTable, relationshipsResult, firstArgIndex, secondArgIndex);

			// If at any point a join returns no entries, the whole query has no solutions, so return false
			if (newCombinedTable.empty()) {
				return false;
			}
		}

			// First clause argument already in table, table join on this common clause argument
		else if (firstArgIndex != -1) {
			newCombinedTable = makeKeyValuePairsAndJoin(combinedTable, relationshipsResult, firstArgIndex,
														KeyColumn::FIRST_COLUMN_KEY);

			// If at any point a join returns no entries, the whole query has no solutions, so return false
			if (newCombinedTable.empty()) {
				return false;
			}

			argumentsInCombinedTable->push_back(relationshipsResult.getSecondArg());
		}

			// Second clause argument already in table, table join on this common clause argument
		else {
			newCombinedTable = makeKeyValuePairsAndJoin(combinedTable, relationshipsResult, secondArgIndex,
														KeyColumn::SECOND_COLUMN_KEY);

			// If at any point a join returns no entries, the whole query has no solutions, so return false
			if (newCombinedTable.empty()) {
				return false;
			}

			argumentsInCombinedTable->push_back(relationshipsResult.getFirstArg());
		}

		// Set new table
		*combinedTable = newCombinedTable;
		return true;
	}

	vector<vector<PQLEntity>> makeKeyValuePairsAndJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
													   RelationshipClauseResult relationshipsResult,
													   int firstArgIndex, int secondArgIndex) {
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
				*combinedTable, firstArgIndex, secondArgIndex);

		vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getTable(relationshipsResult);

		return this->pairKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
	}

	vector<vector<PQLEntity>> makeKeyValuePairsAndJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
													   RelationshipClauseResult relationshipsResult,
													   int argIndex, KeyColumn keyColumn) {

		vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
				*combinedTable, argIndex);

		vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getKeyValueTable(relationshipsResult, keyColumn);

		return this->singleKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
	}



public:
    QueryEvaluator() {};

	/* Returns the final result of a query */
	set<string> evaluate(Query query, shared_ptr<PKBQueryHandler> pkb);

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
