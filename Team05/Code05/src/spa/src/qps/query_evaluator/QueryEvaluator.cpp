#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<EntityClauseResult>> entitiesResultPointers = query.executeSelect(pkb);
	list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers = query.executeSuchThatAndPattern(pkb);
	return combine(entitiesResultPointers, relationshipsResultPointers);
}

set<string> QueryEvaluator::combine(list<shared_ptr<EntityClauseResult>> entitiesResultPointers,
									list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers) {

	list<EntityClauseResult> entitiesResults;
	for (shared_ptr<EntityClauseResult> entitiesResultPointer : entitiesResultPointers) {
		entitiesResults.push_back(*entitiesResultPointer);
	}

	list<RelationshipClauseResult> relationshipsResults;
	for (shared_ptr<RelationshipClauseResult> relationshipsResultPointer : relationshipsResultPointers) {
		relationshipsResults.push_back(*relationshipsResultPointer);
	}

    // If result from SelectClause returns no entries, return empty set
	for (EntityClauseResult entitiesResult : entitiesResults) {
		if (entitiesResult.isEmpty()) {
			return set<string>();
		}
	}

    // If result from any other Clause returns no entries, return empty set
    for (RelationshipClauseResult relationshipsResult : relationshipsResults) {
        if (relationshipsResult.isEmpty()) {
            return set<string>();
        }
    }

	// Filter results to get set of entities that meet all constraints
    vector<vector<PQLEntity>> entitiesToReturn = filterEntities(entitiesResults, relationshipsResults);

	// Convert set of entities into set of entity strings
	set<string> entityStringsToReturn;

	if (entitiesToReturn.size() > 0) {
		for (int columnIndex = 0; columnIndex < entitiesToReturn[0].size(); columnIndex++) {
			string row;
			for (int rowIndex = 0; rowIndex < entitiesToReturn.size(); rowIndex++) {
				PQLEntity entity = entitiesToReturn[rowIndex][columnIndex];
				if (rowIndex != 0) {
					row += " ";
				}
				row += entity.toString();
			}
			entityStringsToReturn.insert(row);
		}
	}

	return entityStringsToReturn;
}

vector<vector<PQLEntity>> QueryEvaluator::filterEntities(list<EntityClauseResult> entitiesResults,
											list<RelationshipClauseResult> relationshipsResults) {

	shared_ptr<vector<vector<PQLEntity>>> combinedTable =
			shared_ptr<vector<vector<PQLEntity>>>(new vector<vector<PQLEntity>>());

	// vector to keep track of arguments already in combinedTable
	shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable =
			shared_ptr<vector<ClauseArgument>>(new vector<ClauseArgument>());

	bool matchingClauseFound = false;

	set<ClauseArgument> selectArgs;
	for (EntityClauseResult result : entitiesResults) {
		selectArgs.insert(result.getArg());
	}

	// Find constraint clause with a clause argument matching select, initialise currentTable
	for (RelationshipClauseResult relationshipsResult : relationshipsResults) {
		bool isFirstArgMatching = selectArgs.find(relationshipsResult.getFirstArg()) != selectArgs.end();
		bool isSecondArgMatching = selectArgs.find(relationshipsResult.getSecondArg()) != selectArgs.end();
		if (isFirstArgMatching || isSecondArgMatching) {

			// Key or value does not matter at this point, arbitrarily set first column to be key
			vector<vector<PQLEntity>> tableToAdd = this->getTable(relationshipsResult);
			*combinedTable = tableToAdd;

			// Add clause arguments from this RelationshipClauseResult to vector for keeping track of columns in combined table
			argumentsInCombinedTable->push_back(relationshipsResult.getFirstArg());
			argumentsInCombinedTable->push_back(relationshipsResult.getSecondArg());

			// Remove since it has been added to the combined table
			relationshipsResults.remove(relationshipsResult);

			matchingClauseFound = true;
			break;
		}
	}

	// Join tables
	bool hasEntries = combinedTableJoin(combinedTable, argumentsInCombinedTable, relationshipsResults);

	// If at any point a table join returns no entries, return no entries
	if (!hasEntries) {
		return vector<vector<PQLEntity>>{};
	}

	// TODO: Implement getting cross product of all entity lists for returning multiple return values
	// If combinedTable is still empty, means no clause has matching arguments but joining all clauses evaluates to true: return select entity list
	if (!matchingClauseFound) {
		return vector<vector<PQLEntity>>{entitiesResults.front().getEntities()};
	}

	// From combined table, pull out column of entities that match select clause argument
	vector<PQLEntity> entitiesToReturn;

	// TODO: Implement cross product of intermediate table so final combined table has all arguments
	// In the combined table, find the column with clause argument matching the select clause argument
	for (int i = 0; i < argumentsInCombinedTable->size(); i++) {
		if (entitiesResults.front().hasArg(argumentsInCombinedTable->at(i))) {
			// For each row, get the PQLEntity in that column and add to entitiesToReturn
			for (vector<PQLEntity> row : *combinedTable) {
				entitiesToReturn.push_back(row[i]);
			}
			break;
		}
	}

	return vector<vector<PQLEntity>>{entitiesToReturn};
}

bool QueryEvaluator::combinedTableJoin(shared_ptr<vector<vector<PQLEntity>>> combinedTable,
									   shared_ptr<vector<ClauseArgument>> argumentsInCombinedTable,
									   list<RelationshipClauseResult> relationshipsResults) {

	list<RelationshipClauseResult> resultsLeft = relationshipsResults;
	while (!resultsLeft.empty()) {

		shared_ptr<list<RelationshipClauseResult>> resultsSkipped = make_shared<list<RelationshipClauseResult>>();

		list<RelationshipClauseResult>::iterator resultIter = resultsLeft.begin();
		for (; resultIter != resultsLeft.end(); resultIter++) {

			bool doesTableHaveEntries = this->createNewTable(combinedTable, argumentsInCombinedTable, resultsSkipped, *resultIter);
			if (!doesTableHaveEntries) {
				return false;
			}

		}

		// If with one iteration of the for loop, the remaining results have not been joined with combinedTable, treat them as boolean
		if (resultsLeft == *resultsSkipped) {
			// If there is one result left that cannot be joined with combinedTable, it can be treated as true and ignored
			// Hence, combinedTable is the final table, return true
			if (resultsLeft.size() == 1) {
				return true;
			}
			// If there are two or more results left, there is a possibility that joining these could return no entries
			// Hence, we recurse and join these tables with combineTableJoin
			else {
				// Data in these fields are not needed, only the end result boolean of the recursive call
				shared_ptr<vector<vector<PQLEntity>>> innerCombinedTable =
						shared_ptr<vector<vector<PQLEntity>>>(new vector<vector<PQLEntity>>());
				shared_ptr<vector<ClauseArgument>> innerArguments =
						shared_ptr<vector<ClauseArgument>>(new vector<ClauseArgument>());
				return combinedTableJoin(innerCombinedTable, innerArguments, resultsLeft);
			}
		}

		// If not, repeat
		resultsLeft = *resultsSkipped;
	}

	// If all results are joined, combinedTable is the final table, return true;
	return true;
}

vector<vector<PQLEntity>> QueryEvaluator::getTable(RelationshipClauseResult relationshipsResult) {

	return this->getKeyValueTable(relationshipsResult, KeyColumn::FIRST_COLUMN_KEY);
}

vector<vector<PQLEntity>> QueryEvaluator::getKeyValueTable(RelationshipClauseResult relationshipsResult, KeyColumn keyColumn) {

	vector<PQLRelationship> relationships = relationshipsResult.getRelationships();
	vector<vector<PQLEntity>> output;

	// For output table, standardise first column as key, second column as value
	for (PQLRelationship relationship : relationships) {
		if (keyColumn == KeyColumn::FIRST_COLUMN_KEY) {
			output.push_back(vector<PQLEntity>{relationship.getFirstEntity(), relationship.getSecondEntity()});
		} else if (keyColumn == KeyColumn::SECOND_COLUMN_KEY) {
			output.push_back(vector<PQLEntity>{relationship.getSecondEntity(), relationship.getFirstEntity()});
		} else {
			throw PQLLogicError("getKeyValueTable: invalid keyColumn");
		}
	}

	return output;
}

int QueryEvaluator::findArgumentIndex(vector<ClauseArgument> argumentsInTable, ClauseArgument argToFind) {
	int argIndex = -1;
	if (!argToFind.isWildcard()) {
		for (int i = 0; i < argumentsInTable.size(); i++) {
			if (argumentsInTable[i] == argToFind) {
				argIndex = i;
				break;
			}
		}
	}
	return argIndex;
}

vector<pair<PQLEntity, vector<PQLEntity>>> QueryEvaluator::convertToKeyValuePairs(vector<vector<PQLEntity>> table, int key) {
	if (table.empty()) {
		throw PQLLogicError("convertToKeyValuePairs: Cannot convert empty table");
	}
	if (key < 0 || key >= table[0].size()) {
		throw PQLLogicError("convertToKeyValuePairs: Key provided out of range of table columns");
	}
	vector<pair<PQLEntity, vector<PQLEntity>>> keyValuePairs;
	for (vector<PQLEntity> row : table) {
		PQLEntity keyEntity = row[key];
		keyValuePairs.emplace_back(keyEntity, row);
	}
	return keyValuePairs;
}

vector<pair<vector<PQLEntity>, vector<PQLEntity>>> QueryEvaluator::convertToKeyValuePairs(vector<vector<PQLEntity>> table, int firstKey, int secondKey) {
	if (table.empty()) {
		throw PQLLogicError("convertToKeyValuePairs: Cannot convert empty table");
	}
	if (firstKey < 0 || firstKey >= table[0].size()) {
		throw PQLLogicError("convertToKeyValuePairs: First key provided out of range of table columns");
	}
	if (secondKey < 0 || secondKey >= table[0].size()) {
		throw PQLLogicError("convertToKeyValuePairs: Second key provided out of range of table columns");
	}
	if (firstKey == secondKey) {
		throw PQLLogicError("convertToKeyValuePairs: Both keys have the same value");
	}
	vector<pair<vector<PQLEntity>, vector<PQLEntity>>> keyValuePairs;
	for (vector<PQLEntity> row : table) {
		PQLEntity firstKeyEntity = row[firstKey];
		PQLEntity secondKeyEntity = row[secondKey];
		vector<PQLEntity> keyEntities = {firstKeyEntity, secondKeyEntity};
		keyValuePairs.emplace_back(keyEntities, row);
	}
	return keyValuePairs;
}

vector<vector<PQLEntity>> QueryEvaluator::pairKeyTableJoin(
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs,
		vector<vector<PQLEntity>> tableToMergeKeyValuePairs) {

	if (combinedTableKeyValuePairs.empty()) {
		throw PQLLogicError("pairKeyTableJoin: Cannot convert empty combinedTable");
	}
	if (tableToMergeKeyValuePairs.empty()) {
		throw PQLLogicError("pairKeyTableJoin: Cannot convert empty tableToMerge");
	}

	// Create multimap
	multimap<vector<PQLEntity>, vector<PQLEntity>> combinedTableMap;

	// Add combinedTable key-value pairs to multimap
	for (pair<vector<PQLEntity>, vector<PQLEntity>> keyValuePair : combinedTableKeyValuePairs) {
		combinedTableMap.insert(keyValuePair);
	}

	// Create output table
	vector<vector<PQLEntity>> output;

	for (vector<PQLEntity> row : tableToMergeKeyValuePairs) {
		// For each key in the tableToMerge, find set of values with this same key in combinedTable (multimap uses a range)
		pair<multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator, multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator> range;
		range = combinedTableMap.equal_range(row);

		// Each value in range is an entry in new table, so add to new table
		for (multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			vector<PQLEntity> rowToAdd = mapIter->second;
			output.push_back(rowToAdd);
		}
	}
	return output;
}

vector<vector<PQLEntity>> QueryEvaluator::singleKeyTableJoin(
		vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs,
		vector<vector<PQLEntity>> tableToMergeKeyValuePairs) {

	if (combinedTableKeyValuePairs.empty()) {
		throw PQLLogicError("singleKeyTableJoin: Cannot convert empty combinedTable");
	}
	if (tableToMergeKeyValuePairs.empty()) {
		throw PQLLogicError("singleKeyTableJoin: Cannot convert empty tableToMerge");
	}

	// Create multimap
	multimap<PQLEntity, vector<PQLEntity>> combinedTableMap;

	// Add combinedTable key-value pairs to multimap
	for (pair<PQLEntity, vector<PQLEntity>> keyValuePair : combinedTableKeyValuePairs) {
		combinedTableMap.insert(keyValuePair);
	}

	// Create output table
	vector<vector<PQLEntity>> output;

	for (vector<PQLEntity> row : tableToMergeKeyValuePairs) {
		// For each key in the tableToMerge, find set of values with this same key in combinedTable (multimap uses a range)
		pair<multimap<PQLEntity, vector<PQLEntity>>::iterator, multimap<PQLEntity, vector<PQLEntity>>::iterator> range;
		range = combinedTableMap.equal_range(row[0]);

		// Each value in range combined with corresponding value in tableToMerge is an entry in new table, so add to new table
		for (multimap<PQLEntity, vector<PQLEntity>>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			vector<PQLEntity> rowToAdd = mapIter->second;
			rowToAdd.push_back(row[1]);
			output.push_back(rowToAdd);
		}
	}
	return output;
}

