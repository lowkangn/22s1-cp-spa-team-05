#include <qps/query/clause/ClauseResult.h>

// =========================================================== //
// ==================== PROTECTED METHODS ==================== //
// =========================================================== //

vector<ClauseArgument> ClauseResult::findConnectingArgs(ClauseResult otherResult) {
	unordered_set<ClauseArgument> argSet;
	for (ClauseArgument arg : this->args) {
		argSet.insert(arg);
	}

	vector<ClauseArgument> connectingArgs;
	for (ClauseArgument arg : otherResult.args) {
		if (!arg.isWildcard() && argSet.find(arg) != argSet.end()) {
			connectingArgs.push_back(arg);
		}
	}
	return connectingArgs;
}

vector<int> ClauseResult::getColumnIndices(vector<ClauseArgument> searchArgs) {
	unordered_map<ClauseArgument, int> argMap;
	for (int i = 0; i < this->args.size(); i++) {
		argMap.insert({this->args[i], i});
	}

	vector<int> indices;
	for (ClauseArgument arg : searchArgs) {
		unordered_map<ClauseArgument, int>::iterator mapIter = argMap.find(arg);
		if (mapIter != argMap.end()) {
			indices.push_back(mapIter->second);
		} else {
			indices.push_back(-1);
		}
	}
	return indices;
}

ClauseResult ClauseResult::performInnerJoin(ClauseResult otherResult, vector<ClauseArgument> connectingArgs) {
	// Get indices of connecting args from each table
	vector<int> thisTableKeyColumnIndices = this->getColumnIndices(connectingArgs);
	vector<int> otherTableKeyColumnIndices = otherResult.getColumnIndices(connectingArgs);
	unordered_set<int> otherTableKeyColumnIndicesSet; // needed later for checking which elements to append
	for (int index : otherTableKeyColumnIndices) {
		otherTableKeyColumnIndicesSet.insert(index);
	}

	// Add this table's entries to multimap: keys are set of elements in key column indices
	multimap<Key, Row> tableMultimap;
	for (Row thisTableRow : this->table) {
		Key key;
		for (int columnIndex : thisTableKeyColumnIndices) {
			key.push_back(thisTableRow[columnIndex]);
		}
		tableMultimap.insert({key, thisTableRow});
	}

	// Create new table and set up arguments (columns)
	ClauseResult newResult = ClauseResult();
	newResult.args = this->args;
	for (int i = 0; i < otherResult.args.size(); i++) {
		if (otherTableKeyColumnIndicesSet.find(i) == otherTableKeyColumnIndicesSet.end()) {
			newResult.addArgumentToTable(otherResult.args[i]);
		}
	}

	// For each entry in other table, search for matching keys in multimap
	for (Row otherTableRow : otherResult.table) {
		Key key;
		for (int columnIndex : otherTableKeyColumnIndices) {
			key.push_back(otherTableRow[columnIndex]);
		}

		pair<multimap<Key, Row>::iterator, multimap<Key, Row>::iterator> range = tableMultimap.equal_range(key);
		for (multimap<Key, Row>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			// For each entry that with matching keys, append elements that aren't already in the entry and add new row to the new table
			Row newRow = mapIter->second;
			for (int i = 0; i < otherTableRow.size(); i++) {
				if (otherTableKeyColumnIndicesSet.find(i) == otherTableKeyColumnIndicesSet.end()) {
					newRow.push_back(otherTableRow[i]);
				}
			}
			newResult.addRowToTable(newRow);
		}
	}

	return newResult;
}

ClauseResult ClauseResult::performCrossProduct(ClauseResult otherResult) {
	// Create new table and set up arguments (columns)
	ClauseResult newResult = ClauseResult();
	for (ClauseArgument arg : this->args) {
		newResult.addArgumentToTable(arg);
	}
	for (ClauseArgument arg : otherResult.args) {
		newResult.addArgumentToTable(arg);
	}

	// Create new rows and add to new table
	for (Row thisTableRow : this->table) {
		for (Row otherTableRow : otherResult.table) {
			Row newRow = thisTableRow;
			newRow.insert(newRow.end(), otherTableRow.begin(), otherTableRow.end());
			newResult.addRowToTable(newRow);
		}
	}

	return newResult;
}

void ClauseResult::addColumn(ClauseResult resultToAdd) {
	if (this->canAddColumn(resultToAdd)) {
		// Add new args
		vector<ClauseArgument> newArgs = resultToAdd.args;
		this->args.insert(this->args.end(), newArgs.begin(), newArgs.end());

		// Add columns
		for (int i = 0; i < this->table.size(); i++) {
			Row newRow = this->table[i];
			Row rowToAppend = resultToAdd.table[i];
			newRow.insert(newRow.end(), rowToAppend.begin(), rowToAppend.end());
			this->table[i] = newRow;
		}
	}
}

ClauseResult ClauseResult::getColumn(int columnIndex) {
	Column column;
	for (int rowIndex = 0; rowIndex < this->table.size(); rowIndex++) {
		column.push_back({this->table[rowIndex][columnIndex]});
	}
	return ClauseResult({this->args[columnIndex]}, column);
}

// ======================================================== //
// ==================== PUBLIC METHODS ==================== //
// ======================================================== //

ClauseResult ClauseResult::mergeResult(ClauseResult resultToMerge) {
	// Find common synonyms between results
	vector<ClauseArgument> connectingArgs = this->findConnectingArgs(resultToMerge);

	// If there are connecting args, find common synonym columns and inner join on those, otherwise perform cross product
	if (!connectingArgs.empty()) {
		return this->performInnerJoin(resultToMerge, connectingArgs);
	} else {
		return this->performCrossProduct(resultToMerge);
	}
}

bool ClauseResult::checkSelectArgsInTable(vector<ClauseResult> selectResults) {
	// Get list of args from select results
	vector<ClauseArgument> selectArgs;
	for (ClauseResult selectResult : selectResults) {
		vector<ClauseArgument> currentArgs = selectResult.args;
		selectArgs.insert(selectArgs.end(), currentArgs.begin(), currentArgs.end());
	}

	unordered_set<ClauseArgument> resultArgsSet;
	for (ClauseArgument arg : this->args) {
		resultArgsSet.insert(arg);
	}

	for (ClauseArgument selectArg : selectArgs) {
		if (resultArgsSet.find(selectArg) != resultArgsSet.end()) {
			return true;
		}
	}

	return false;
}

ClauseResult ClauseResult::rearrangeTableToMatchSelectResults(vector<ClauseResult> selectResults) {
	// Get args
	vector<ClauseArgument> selectArgs;
	for (ClauseResult selectResult : selectResults) {
		vector<ClauseArgument> currentArgs = selectResult.args;
		selectArgs.insert(selectArgs.end(), currentArgs.begin(), currentArgs.end());
	}

	// Find corresponding column indices
	vector<int> desiredSynonymIndices = this->getColumnIndices(selectArgs);

	// Map to check if arg not found in combined table has already been merged
	unordered_map<ClauseArgument, int> missingArgMap;

	// If arg not found in combined table, merge respective list from select result
	for (int i = 0; i < desiredSynonymIndices.size(); i++) {
		if (desiredSynonymIndices[i] == -1) {
			ClauseResult currentResult = selectResults[i];
			unordered_map<ClauseArgument, int>::iterator mapIter = missingArgMap.find(currentResult.args[0]);
			if ( mapIter == missingArgMap.end()) {
				ClauseResult newResult = this->mergeResult(currentResult);
				this->args = newResult.args;
				this->table = newResult.table;
				desiredSynonymIndices[i] = int(this->args.size()) - 1;
				missingArgMap.insert({currentResult.args[0], int(this->args.size()) - 1});
			} else {
				desiredSynonymIndices[i] = mapIter->second;
			}
		}
	}

	// If all selected args can be found in combined table, get table of desired synonyms
	Table desiredSynonymsTable;
	for (Row row : this->table) {
		Row newRow;
		for (int index : desiredSynonymIndices) {
			newRow.push_back(row[index]);
		}
		desiredSynonymsTable.push_back(newRow);
	}

	return ClauseResult(selectArgs, desiredSynonymsTable);
}

void ClauseResult::duplicateColumn(ClauseResult column) {
	assert(column.args.size() == 1); // Method is only called with a column as parameter
	ClauseArgument arg = column.args[0];
	int countIndex = this->getColumnIndices({arg})[0];
	assert(countIndex != -1); // Method is only called if column is known to already exist in table
	ClauseResult duplicatedColumn = this->getColumn(countIndex);
	this->addColumn(duplicatedColumn);
}

set<string> ClauseResult::convertTableToString(bool isBooleanReturnType) {
	set<string> entityStringsToReturn;

	if (isBooleanReturnType) {
		if (this->table.empty()) {
			entityStringsToReturn.insert("FALSE");
		} else {
			entityStringsToReturn.insert("TRUE");
		}
	}
	else {
		for (Row row : this->table) {
			string out;
			vector<PQLEntity>::iterator entityIter = row.begin();
			for (; entityIter != row.end(); entityIter++) {
				if (entityIter != row.begin()) {
					out += " ";
				}
				out += entityIter->toString();
			}
			entityStringsToReturn.insert(out);
		}
	}

	return entityStringsToReturn;
}

bool ClauseResult::equals(shared_ptr<ClauseResult> other) {
	return (this->args == other->args) && (this->table == other->table);
}

bool operator<(ClauseResult first, ClauseResult second) {
	return first.args < second.args || (first.args == second.args && first.table < second.table);
}

bool operator==(ClauseResult first, ClauseResult second) {
	return first.args == second.args && first.table == second.table;
}