#include <qps/query/clause/ClauseResult.h>

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
	newResult.addArgumentsToTable(this->args);
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
	newResult.addArgumentsToTable(this->args);
	newResult.addArgumentsToTable(otherResult.args);

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

vector<int> ClauseResult::getColumnIndices(vector<ClauseArgument> args) {
	unordered_map<ClauseArgument, int> argMap;
	for (int i = 0; i < this->args.size(); i++) {
		argMap.insert({this->args[i], i});
	}

	vector<int> indices;
	for (ClauseArgument arg : args) {
		unordered_map<ClauseArgument, int>::iterator mapIter = argMap.find(arg);
		if (mapIter != argMap.end()) {
			indices.push_back(mapIter->second);
		} else {
			indices.push_back(-1);
		}
	}
	return indices;
}

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