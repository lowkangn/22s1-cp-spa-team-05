#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
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
	vector<ClauseArgument> args;
	Table table;

	/* Method for finding common synonyms */
	vector<ClauseArgument> findConnectingArgs(ClauseResult otherResult);

	/* Method for performing inner join (have common synonyms) */
	ClauseResult performInnerJoin(ClauseResult otherResult, vector<ClauseArgument> connectingArgs);

	/* Method for performing cross product (no common synonyms) */
	ClauseResult performCrossProduct(ClauseResult otherResult);

	/* Methods for adding stuff to table */
	void addArgumentToTable(ClauseArgument argumentToAdd) {
		this->args.push_back(argumentToAdd);
	}

	void addArgumentsToTable(vector<ClauseArgument> argumentsToAdd) {
		this->args.insert(this->args.end(), argumentsToAdd.begin(), argumentsToAdd.end());
	}

	void addRowToTable(Row rowToAdd) {
		this->table.push_back(rowToAdd);
	}

	/* Method for checking if column can be added */
	bool canAddColumn(ClauseResult result) {
		return this->table.size() == result.table.size();
	}

public:
	/* ================ Constructors ================ */

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

	/* Gets the indices in the table corresponding to the given args - returns empty if an arg is not found in table */
	vector<int> getColumnIndices(vector<ClauseArgument> args);

	/* Merges another ClauseResult */
	ClauseResult mergeResult(ClauseResult resultToMerge);

	/* Adds a column to the table */
	void addColumn(ClauseResult resultToAdd);

	/* Get column from table by index */
	ClauseResult getColumn(int index);

	/* Get args */
	vector<ClauseArgument> getArgs() {
		return this->args;
	}

	/* Get table */
	Table getTable() {
		return this->table;
	}

	/* Checks if result table is empty */
	bool isEmpty() {
		return this->table.empty();
	}
};
