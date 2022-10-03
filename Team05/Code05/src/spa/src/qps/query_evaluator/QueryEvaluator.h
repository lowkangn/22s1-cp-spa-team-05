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

enum class KeyColumn {
	FIRST_COLUMN_KEY,
	SECOND_COLUMN_KEY
};

class QueryEvaluator {

private:
	/**
	 * Creates a new combined table from the current combined table and a RelationshipClauseResult.
	 *
	 * This process consists of converting the table into key-value pairs based on matching arguments in the
	 * RelationshipClauseResult to facilitate merging, and the actual merging of entries to create the new table.
	 *
	 * @param combinedTable 			Current combined table.
	 * @param argumentsInCombinedTable	Clause arguments currently in combined table (keeps track of columns).
	 * @param resultsSkipped			Vector to add RelationshipClauseResult to if tables cannot be joined.
	 * @param relationshipsResult		The RelationshipClauseResult.
	 *
	 * @return 	true if a non-empty table is created, false otherwise. Used to check if table joining has resulted in an
	 * 			empty table, in which case we can just stop and return no entries.
	 */
	bool createNewTable(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
						shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable,
						shared_ptr<list<RelationshipClauseResult>> resultsSkipped,
						RelationshipClauseResult relationshipsResult) {

		// If the given table is empty, just insert the table from the RelationshipClauseResult
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

	/**
	 * Overloaded method for the process of forming key value pairs from the tables and performing merging. This method
	 * is used when the combinedTable and the RelationshipClauseResult's table have two ClauseArguments in common.
	 * Helper method for createNewTable.
	 *
	 * @param combinedTable 		Current combined table.
	 * @param relationshipsResult 	RelationshipClauseResult whose table is to be joined with combinedTable.
	 * @param firstArgIndex 		Index of first matching ClauseArgument (relationshipsResult's first
	 * 								ClauseArgument's column index in the combined table).
	 * @param secondArgIndex 		Index of second matching ClauseArgument (relationshipsResult's second
	 * 								ClauseArgument's column index in the combined table).
	 *
	 * @return The new combined table.
	 */
	vector<vector<PQLEntity>> makeKeyValuePairsAndJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
													   RelationshipClauseResult relationshipsResult,
													   int firstArgIndex, int secondArgIndex) {
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
				*combinedTable, firstArgIndex, secondArgIndex);

		vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getTable(relationshipsResult);

		return this->pairKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
	}

	/**
	 * Overloaded method for the process of forming key value pairs from the tables and performing merging. This method
	 * is used when the combinedTable and the RelationshipClauseResult's table have one ClauseArgument in common.
	 * Helper method for createNewTable.
	 *
	 * @param combinedTable 		Current combined table.
	 * @param relationshipsResult 	RelationshipClauseResult whose table is to be joined with combinedTable.
	 * @param argIndex 				Index of matching ClauseArgument (column index in the combined table of one of
	 * 								the matching relationshipsResult's ClauseArgument).
	 * @param keyColumn 			Either first or second column depending of which of relationshipsResult's
	 * 								ClauseArguments has a match in the combined table.
	 *
	 * @return The new combined table.
	 */
	vector<vector<PQLEntity>> makeKeyValuePairsAndJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
													   RelationshipClauseResult relationshipsResult,
													   int argIndex, KeyColumn keyColumn) {

		vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
				*combinedTable, argIndex);

		vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getKeyValueTable(relationshipsResult, keyColumn);

		return this->singleKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
	}



public:
	/**
	 * Default constructor for the QueryEvaluator.
	 */
    QueryEvaluator() {};

	/**
	 * Returns the final result of a Query.
	 * @param query to execute.
	 * @param pkb to interact with to extract stored data.
	 * @return a set of strings matching the Query.
	 */
	set<string> evaluate(Query query, shared_ptr<PKBQueryHandler> pkb);

	/**
	 * Combines the results from executing individual Clause objects in a Query.
	 * @param entitiesResultPointer pointing to an EntityClauseResult encapsulating SelectClause execution results.
	 * @param relationshipsResultPointers pointing to RelationshipClauseResults encapsulating RelationshipClause execution results.
	 * @return a set of strings matching the Query.
	 */
	vector<vector<PQLEntity>> combine(list<shared_ptr<EntityClauseResult>> entitiesResultPointer,
						list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers);

	/**
	 * Combines the results from EntityClauseResult and RelationshipClauseResults in a Query. Helper method for combine.
	 * @param entitiesResult encapsulating SelectClause execution results.
	 * @param relationshipsResults encapsulating RelationshipClause execution results.
	 * @return vector of PQLEntity objects that match the Query.
	 */
	vector<vector<PQLEntity>> filterEntities(list<EntityClauseResult> entitiesResults,
									 list<RelationshipClauseResult> relationshipsResults);


	/**
	 * Helper method for filterEntities.
	 *
	 * @param combinedTable Current combined table.
	 * @param argumentsInCombinedTable Arguments currently in the combined table.
	 * @param relationshipsResults to get tables from and join with combined table.
	 * @return 	true if joins return a non-empty table, false otherwise. Used to check if table joining has resulted in an
	 * 			empty table, in which case we can just stop and return no entries.
	 */
	bool combinedTableJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
						   shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable,
						   list<RelationshipClauseResult> relationshipsResults);

	/**
	 * Get a table of PQLEntity objects built from the RelationshipClauseResult's list of PQLRelationship objects.
	 * @param relationshipsResult whose list<PQLRelationship> is to be converted to vector<vector<PQLEntity>>.
	 * @return table of PQLEntity objects.
	 */
	vector<vector<PQLEntity>> getTable(RelationshipClauseResult relationshipsResult);

	/**
	 * Get a table of PQLEntity objects built from the RelationshipClauseResult's list of PQLRelationship objects, with
	 * the specified keyColumn designated as the key (and the other, the value).
	 *
	 * @param relationshipsResult whose list<PQLRelationship> is to be converted to vector<vector<PQLEntity>>.
	 * @param keyColumn column of the table that you want to be treated as the key in a key-value pair.
	 * @returntable of PQLEntity objects.
	 */
	vector<vector<PQLEntity>> getKeyValueTable(RelationshipClauseResult relationshipsResult, KeyColumn keyColumn);

	/**
	 * Finds the index of a ClauseArgument in the combined table. Returns -1 if not found.
	 *
	 * @param argumentsInTable List of arguments in the combined table.
	 * @param argToFind Desired argument.
	 * @return index of desired argument in the combined table.
	 */
	int findArgumentIndex(vector<ClauseArgument> argumentsInTable, ClauseArgument argToFind);

	/**
	 * Forms key-value pairs from the given table and column index. The return is a vector of pairs, each pair has a
	 * PQLEntity as key and vector<PQLEntity> as value. When forming key-value pairs, the key isn't removed from the
	 * rest of the elements (this is to facilitate easier merging during table join later).
	 *
	 * @param table
	 * @param key index of column to be designated as key.
	 * @return vector of key-value pairs.
	 */
	vector<pair<PQLEntity, vector<PQLEntity>>> convertToKeyValuePairs(vector<vector<PQLEntity>> table, int key);

	/**
	 * Forms key-value pairs from the given table and column indexes. The return is a vector of pairs, each pair has a
	 * vector<PQLEntity> as key and vector<PQLEntity> as value. When forming key-value pairs, the key (vector of 2 PQLEntity)
	 * is not removed from the rest of the elements (this is to facilitate easier merging during table join later).
	 *
	 * @param table
	 * @param firstKey index of first column that forms the key.
	 * @param secondKey index of second column that forms the key.
	 * @return vector of key-value pairs.
	 */
	vector<pair<vector<PQLEntity>, vector<PQLEntity>>> convertToKeyValuePairs(vector<vector<PQLEntity>> table,
																			  int firstKey, int secondKey);

	/**
	 * Merges two tables on keys that consist of 2 PQLEntities.
	 *
	 * @param combinedTableKeyValuePairs key-value pairs derived from combinedTable.
	 * @param tableToMergeKeyValuePairs key-value pairs derived from table being joined to combinedTable.
	 * @return new combined table.
	 */
	vector<vector<PQLEntity>> pairKeyTableJoin(
			vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMergeKeyValuePairs);

	/**
	 * Merges two tables on keys that consist of 1 PQLEntity.
	 *
	 * @param combinedTableKeyValuePairs key-value pairs derived from combinedTable.
	 * @param tableToMergeKeyValuePairs key-value pairs derived from table being joined to combinedTable.
	 * @return new combined table.
	 */
	vector<vector<PQLEntity>> singleKeyTableJoin(
			vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs,
			vector<vector<PQLEntity>> tableToMergeKeyValuePairs);

	set<string> convertResultsToString(vector<vector<PQLEntity>> entitiesToReturn, bool isBooleanReturnType);

};
