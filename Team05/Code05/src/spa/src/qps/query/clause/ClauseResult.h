#pragma once

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
#include <algorithm>
#include <iterator>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLRelationship.h>

using namespace std;

typedef vector<PQLEntity> Key;
typedef vector<PQLEntity> Row;
typedef vector<vector<PQLEntity>> Column;
typedef vector<vector<PQLEntity>> Table;

class ClauseResult {
protected:
	/* ================ Protected fields ================ */

	vector<ClauseArgument> args;
	unordered_map<ClauseArgument, int> argumentToIndexMap;
	Table table;

	/* ================ Protected methods ================ */

	/* Method for finding common synonyms */
	vector<ClauseArgument> findConnectingArgs(const ClauseResult& otherResult);

	/* Gets the indices in the table corresponding to the given args - returns empty if an arg is not found in table */
	vector<int> getColumnIndices(const vector<ClauseArgument>& args);

	/* Method for performing inner join (have common synonyms) */
	ClauseResult performInnerJoin(ClauseResult otherResult, const vector<ClauseArgument>& connectingArgs);

	/* Method for performing cross product (no common synonyms) */
	ClauseResult performCrossProduct(const ClauseResult& otherResult);

	/* Method for adding argument to table */
	void addArgumentToTable(const ClauseArgument& argumentToAdd) {
		this->args.push_back(argumentToAdd);
		this->argumentToIndexMap.insert({argumentToAdd, this->args.size() - 1});
	}

	/* Method for adding row to table */
	void addRowToTable(const Row& rowToAdd) {
		this->table.push_back(rowToAdd);
	}

	/* Method for checking if column can be added */
	bool canAddColumnToTable(const ClauseResult& result) {
		return this->table.size() == result.table.size();
	}

	/* Adds a column to the table */
	void addColumnToTable(ClauseResult resultToAdd);

	/* Get column from table by index */
	ClauseResult getColumnFromTable(int index);

	/* Duplicates an existing column of this ClauseResult with the header changed to newColumnHeader. */
	void duplicateExistingColumnAs(const ClauseArgument& headerOfColumnToDuplicate, const ClauseArgument& newColumnHeader);

	/* Renames columns currently named oldName to newName */
	void renameColumns(const ClauseArgument& oldName, const ClauseArgument& newName);

	/* Converts a column of synonyms to their corresponding attributes given a select result */
	ClauseResult convertSynonymsColumnToAttributesColumn(ClauseResult selectResult);

public:
	/* ============= Public constructors ============= */

	ClauseResult() {};

	ClauseResult(vector<ClauseArgument> args, vector<vector<PQLEntity>> table) : args(args), table(table) {
		for (int i = 0; i < args.size(); i++) {
			this->argumentToIndexMap.insert({args[i], i});
		}
	};

	ClauseResult(vector<ClauseArgument> args, const vector<PQLEntity>& entities) : args(args) {
		for (const PQLEntity& entity : entities) {
			table.push_back(Row{entity});
		}
		for (int i = 0; i < args.size(); i++) {
			this->argumentToIndexMap.insert({args[i], i});
		}
	};

	ClauseResult(vector<ClauseArgument> args, const vector<PQLRelationship>& relationships) : args(args) {
		for (PQLRelationship relationship : relationships) {
			table.push_back(Row{relationship.getFirstEntity(), relationship.getSecondEntity()});
		}
		for (int i = 0; i < args.size(); i++) {
			this->argumentToIndexMap.insert({args[i], i});
		}
	};

	/* ================ Public methods ================ */

	/* Merges another ClauseResult */
	ClauseResult mergeResult(const ClauseResult& resultToMerge);

	/* Merges this ClauseResult with another ClauseResult by performing an inner join on the given join columns.
		Keeps both join columns. */
	ClauseResult mergeByForceInnerJoin(ClauseResult resultToMerge, const ClauseArgument& leftOn, const ClauseArgument& rightOn);

	/* Checks if any of the given args are in the table */
	bool checkSelectArgsInTable(const vector<ClauseResult>& selectResults);

	/* Gets table rearranged based on args from select results */
	ClauseResult rearrangeTableToMatchSelectResults(const vector<ClauseResult>& selectResults);

	/* Duplicates an existing column and adds it to the table */
	void duplicateColumn(ClauseResult column);

	/* Convert table to set of strings (output) */
	set<string> convertTableToString(bool isBooleanReturnType);

	/* Checks if result table is empty */
	bool isEmpty() {
		return this->table.empty();
	}

	vector<ClauseArgument> getSynonymArgs() {
		vector<ClauseArgument> result;
		copy_if(this->args.begin(), this->args.end(), back_inserter(result),
				[](ClauseArgument arg) {return arg.isSynonym();});
		return result;
	}

	virtual bool equals(shared_ptr<ClauseResult> other);

	friend bool operator<(ClauseResult first, ClauseResult second);

	friend bool operator==(ClauseResult first, ClauseResult second);
};
