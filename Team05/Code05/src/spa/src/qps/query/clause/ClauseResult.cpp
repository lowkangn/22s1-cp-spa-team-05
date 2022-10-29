#include <qps/query/clause/ClauseResult.h>

// =========================================================== //
// ==================== PROTECTED METHODS ==================== //
// =========================================================== //

vector<ClauseArgument> ClauseResult::findConnectingArgs(const ClauseResult& otherResult) {
	vector<ClauseArgument> connectingArgs;
	for (ClauseArgument arg : otherResult.args) {
		if (!arg.isWildcard() && this->argumentToIndexMap.find(arg) != this->argumentToIndexMap.end()) {
			connectingArgs.push_back(arg);
		}
	}
	return connectingArgs;
}

vector<int> ClauseResult::getColumnIndices(const vector<ClauseArgument>& searchArgs) {
	vector<int> indices;
	for (const ClauseArgument& arg : searchArgs) {
		unordered_multimap<ClauseArgument, int>::iterator mapIter = this->argumentToIndexMap.find(arg);
		if (mapIter != this->argumentToIndexMap.end()) {
			indices.push_back(mapIter->second);
		} else {
			indices.push_back(-1);
		}
	}
	return indices;
}

ClauseResult ClauseResult::performInnerJoin(ClauseResult otherResult, const vector<ClauseArgument>& connectingArgs) {
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
	newResult.argumentToIndexMap = this->argumentToIndexMap;
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

ClauseResult ClauseResult::performCrossProduct(const ClauseResult& otherResult) {
	// Create new table and set up arguments (columns)
	ClauseResult newResult = ClauseResult();
	for (const ClauseArgument& arg : this->args) {
		newResult.addArgumentToTable(arg);
	}
	for (const ClauseArgument& arg : otherResult.args) {
		newResult.addArgumentToTable(arg);
	}

	// Create new rows and add to new table
	for (const Row& thisTableRow : this->table) {
		for (Row otherTableRow : otherResult.table) {
			Row newRow = thisTableRow;
			newRow.insert(newRow.end(), otherTableRow.begin(), otherTableRow.end());
			newResult.addRowToTable(newRow);
		}
	}

	return newResult;
}

void ClauseResult::addColumnToTable(ClauseResult resultToAdd) {
	if (this->canAddColumnToTable(resultToAdd)) {
		// Add new args
		for (const ClauseArgument& newArg : resultToAdd.args) {
			this->addArgumentToTable(newArg);
		}

		// Add columns
		for (int i = 0; i < this->table.size(); i++) {
			Row newRow = this->table[i];
			Row rowToAppend = resultToAdd.table[i];
			newRow.insert(newRow.end(), rowToAppend.begin(), rowToAppend.end());
			this->table[i] = newRow;
		}
	}
}

ClauseResult ClauseResult::getColumnFromTable(int columnIndex) {
	Column column;
	for (vector<PQLEntity> rowIndex : this->table) {
		column.push_back({rowIndex[columnIndex]});
	}
	return ClauseResult({this->args[columnIndex]}, column);
}

void ClauseResult::duplicateExistingColumnAs(const ClauseArgument& headerOfColumnToDuplicate, const ClauseArgument& newColumnHeader) {
	vector<int> indices = this->getColumnIndices(vector<ClauseArgument>{ headerOfColumnToDuplicate });
	assert(indices.size() == 1 && indices.front() != -1); //column must be an existing column

	ClauseResult columnToDuplicate = this->getColumnFromTable(indices.front());
	columnToDuplicate.renameColumns(headerOfColumnToDuplicate, newColumnHeader);
	this->addColumnToTable(columnToDuplicate);
}

void ClauseResult::renameColumns(const ClauseArgument& oldName, const ClauseArgument& newName) {
	replace(this->args.begin(), this->args.end(), oldName, newName);
	if (this->argumentToIndexMap.count(oldName)) {
		this->argumentToIndexMap.insert({newName, this->argumentToIndexMap.find(oldName)->second});
	}
	this->argumentToIndexMap.erase(oldName);
}

ClauseResult ClauseResult::convertSynonymsColumnToAttributesColumn(ClauseResult selectResult) {
	unordered_map<PQLEntity, PQLEntity> synonymAttributeMap;
	assert(this->args.size() == 1);
	assert(selectResult.args.size() == 2);
	for (Row row : selectResult.table) {
		synonymAttributeMap.insert({row[0], row[1]});
	}

	Column newColumn;
	for (Row row : this->table) {
		unordered_map<PQLEntity, PQLEntity>::iterator mapIter = synonymAttributeMap.find(row[0]);
		assert(mapIter != synonymAttributeMap.end());
		newColumn.push_back({mapIter->second});
	}

	return ClauseResult({selectResult.args[1]}, newColumn);
}

// ======================================================== //
// ==================== PUBLIC METHODS ==================== //
// ======================================================== //

ClauseResult ClauseResult::mergeResult(const ClauseResult& resultToMerge) {
	// Find common synonyms between results
	vector<ClauseArgument> connectingArgs = this->findConnectingArgs(resultToMerge);

	// If there are connecting args, find common synonym columns and inner join on those, otherwise perform cross product
	if (!connectingArgs.empty()) {
		return this->performInnerJoin(resultToMerge, connectingArgs);
	} else {
		return this->performCrossProduct(resultToMerge);
	}
}

ClauseResult ClauseResult::mergeByForceInnerJoin(ClauseResult resultToMerge, const ClauseArgument& leftOn, const ClauseArgument& rightOn) {
	bool leftHasArg = *find(this->args.begin(), this->args.end(), leftOn) == leftOn;
	bool rightHasArg = *find(resultToMerge.args.begin(), resultToMerge.args.end(), rightOn) == rightOn;
	if (!leftHasArg || !rightHasArg) {
		throw PQLLogicError("ClauseResults must have the columns to force inner join on");
	}

	string leftPlaceholderSuffix = "left";
	string rightPlaceholderSuffix = "right";

	// temporarily rename any possible natural join synonyms 
	vector<ClauseArgument> connectingArgs = this->findConnectingArgs(resultToMerge);
	int i = 0;
	for (const ClauseArgument& arg : connectingArgs) {
		if (arg != leftOn) {
			this->renameColumns(arg, ClauseArgument::createStmtArg(to_string(i) + leftPlaceholderSuffix));
		}
		if (arg != rightOn) {
			resultToMerge.renameColumns(arg, ClauseArgument::createStmtArg(to_string(i) + rightPlaceholderSuffix));
		}
		i++;
	}
	// perform join
	resultToMerge.duplicateExistingColumnAs(rightOn, leftOn);
	ClauseResult merged = this->performInnerJoin(resultToMerge, vector<ClauseArgument>{ leftOn });
	// undo temporary renaming
	i = 0;
	for (const ClauseArgument& arg : connectingArgs) {
		merged.renameColumns(ClauseArgument::createStmtArg(to_string(i) + leftPlaceholderSuffix), arg);
		merged.renameColumns(ClauseArgument::createStmtArg(to_string(i) + rightPlaceholderSuffix), arg);
		i++;
	}
	return merged;
}

bool ClauseResult::checkSelectArgsInTable(const vector<ClauseResult>& selectResults) {
	// Get list of args from select results
	vector<ClauseArgument> selectArgs;
	for (const ClauseResult& selectResult : selectResults) {
		vector<ClauseArgument> currentArgs = selectResult.args;
		selectArgs.insert(selectArgs.end(), currentArgs.begin(), currentArgs.end());
	}

	for (const ClauseArgument& selectArg : selectArgs) {
		if (this->argumentToIndexMap.find(selectArg) != this->argumentToIndexMap.end()) {
			return true;
		}
	}

	return false;
}

ClauseResult ClauseResult::rearrangeTableToMatchSelectResults(const vector<ClauseResult>& selectResults) {
	// Vector to keep track of which columns contain desired results
	vector<int> desiredSynonymIndices;

	for (const ClauseResult& currentResult : selectResults) {
		vector<int> currentDesiredSynonymIndices = this->getColumnIndices(currentResult.args);
		assert(currentDesiredSynonymIndices.size() == 1 || currentDesiredSynonymIndices.size() == 2);

		// Select arg has one column: default attribute
		if (currentDesiredSynonymIndices.size() == 1) {
			// If in table, return index
			if (currentDesiredSynonymIndices[0] != -1) {
				desiredSynonymIndices.push_back(currentDesiredSynonymIndices[0]);
			}
			// If not in table, merge
			else {
				ClauseResult newResult = this->performCrossProduct(currentResult);
				this->args = newResult.args;
				this->argumentToIndexMap = newResult.argumentToIndexMap;
				this->table = newResult.table;
				int indexOfNewColumn = static_cast<int>(this->args.size()) - 1;
				desiredSynonymIndices.push_back(indexOfNewColumn);
			}
		}

		// Select arg has two columns: non-default attribute in second column
		else {
			// If in table, return index to attribute column
			if (currentDesiredSynonymIndices[0] != -1 && currentDesiredSynonymIndices[1] != -1) {
				desiredSynonymIndices.push_back(currentDesiredSynonymIndices[1]);
			}
			// If not in table, merge
			else {
				if (currentDesiredSynonymIndices[0] == -1) {
					ClauseResult newResult = this->performCrossProduct(currentResult);
					this->args = newResult.args;
					this->argumentToIndexMap = newResult.argumentToIndexMap;
					this->table = newResult.table;
				} else {
					ClauseResult synonymColumn = this->getColumnFromTable(currentDesiredSynonymIndices[0]);
					ClauseResult attributeColumn = synonymColumn.convertSynonymsColumnToAttributesColumn(currentResult);
					this->addColumnToTable(attributeColumn);
				}
				int indexOfNewColumn = static_cast<int>(this->args.size()) - 1;
				desiredSynonymIndices.push_back(indexOfNewColumn);
			}
		}
	}

	// Obtain select args
	vector<ClauseArgument> selectArgs;
	for (ClauseResult selectResult : selectResults) {
		assert(selectResult.args.size() == 1 || selectResult.args.size() == 2);
		if (selectResult.args.size() == 1) {
			selectArgs.push_back(selectResult.args[0]);
		} else {
			selectArgs.push_back(selectResult.args[1]);
		}
	}

	// All selected args should now be in combined table, so get table of desired synonyms
	Table desiredSynonymsTable;
	for (Row row : this->table) {
		Row newRow;
		for (int index : desiredSynonymIndices) {
			newRow.push_back(row[index]);
		}
		desiredSynonymsTable.push_back(newRow);
	}

	return {selectArgs, desiredSynonymsTable};
}

void ClauseResult::duplicateColumn(ClauseResult column) {
	assert(column.args.size() == 1); // Method is only called with a column as parameter
	ClauseArgument arg = column.args[0];
	int countIndex = this->getColumnIndices({arg})[0];
	assert(countIndex != -1); // Method is only called if column is known to already exist in table
	ClauseResult duplicatedColumn = this->getColumnFromTable(countIndex);
	this->addColumnToTable(duplicatedColumn);
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
	Table firstTable = this->table;
	Table secondTable = other->table;
	sort(firstTable.begin(), firstTable.end());
	sort(secondTable.begin(), secondTable.end());
	return (this->args == other->args) && (firstTable == secondTable);
}

bool operator<(ClauseResult first, ClauseResult second) {
	return first.args < second.args || (first.args == second.args && first.table < second.table);
}

bool operator==(ClauseResult first, ClauseResult second) {
	return first.equals(make_shared<ClauseResult>(second));
}
