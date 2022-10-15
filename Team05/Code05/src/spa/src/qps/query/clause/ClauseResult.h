#pragma once

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
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
	Table table;

	/* ================ Protected methods ================ */

	/* Method for finding common synonyms */
	vector<ClauseArgument> findConnectingArgs(ClauseResult otherResult);

	/* Gets the indices in the table corresponding to the given args - returns empty if an arg is not found in table */
	vector<int> getColumnIndices(vector<ClauseArgument> args);

	/* Method for performing inner join (have common synonyms) */
	ClauseResult performInnerJoin(ClauseResult otherResult, vector<ClauseArgument> connectingArgs);

	/* Method for performing cross product (no common synonyms) */
	ClauseResult performCrossProduct(ClauseResult otherResult);

	/* Method for adding argument to table */
	void addArgumentToTable(ClauseArgument argumentToAdd) {
		this->args.push_back(argumentToAdd);
	}

	/* Method for adding row to table */
	void addRowToTable(Row rowToAdd) {
		this->table.push_back(rowToAdd);
	}

	/* Method for checking if column can be added */
	bool canAddColumn(ClauseResult result) {
		return this->table.size() == result.table.size();
	}

	/* Adds a column to the table */
	void addColumn(ClauseResult resultToAdd);

	/* Get column from table by index */
	ClauseResult getColumn(int index);

public:
	/* ============= Public constructors ============= */

	ClauseResult() {};

	ClauseResult(vector<ClauseArgument> args, vector<vector<PQLEntity>> table) : args(args), table(table) {};

	ClauseResult(vector<ClauseArgument> args, vector<PQLEntity> entities) : args(args) {
		for (PQLEntity entity : entities) {
			table.push_back(Row{entity});
		}
	};

	ClauseResult(vector<ClauseArgument> args, vector<PQLRelationship> relationships) : args(args) {
		for (PQLRelationship relationship : relationships) {
			table.push_back(Row{relationship.getFirstEntity(), relationship.getSecondEntity()});
		}
	};

	/* ================ Public methods ================ */

	/* Merges another ClauseResult */
	ClauseResult mergeResult(ClauseResult resultToMerge);

	/* Checks if any of the given args are in the table */
	bool checkSelectArgsInTable(vector<ClauseResult> selectResults);

	/* Gets table rearranged based on args from select results */
	ClauseResult rearrangeTableToMatchSelectResults(vector<ClauseResult> selectResults);

	/* Duplicates an existing column and adds it to the table */
	void duplicateColumn(ClauseResult column);

	/* Convert table to set of strings (output) */
	set<string> convertTableToString(bool isBooleanReturnType);

	/* Checks if result table is empty */
	bool isEmpty() {
		return this->table.empty();
	}

	virtual bool equals(shared_ptr<ClauseResult> other);

	friend bool operator<(ClauseResult first, ClauseResult second);

	friend bool operator==(ClauseResult first, ClauseResult second);
};
