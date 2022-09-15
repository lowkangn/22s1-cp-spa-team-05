#include <set>
#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKB> pkb) {
	shared_ptr<EntityClauseResult> entitiesResultPointer = query.executeSelect(pkb);
	list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers = query.executeSuchThat(pkb);
	return combine(entitiesResultPointer, relationshipsResultPointers);
}

set<string> QueryEvaluator::combine(shared_ptr<EntityClauseResult> entitiesResultPointer,
									list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers) {

    EntityClauseResult entitiesResult = *entitiesResultPointer;

	list<RelationshipClauseResult> relationshipsResults;
	for (shared_ptr<RelationshipClauseResult> relationshipsResultPointer : relationshipsResultPointers) {
		relationshipsResults.push_back(*relationshipsResultPointer);
	}

    // If result from SelectClause returns no entries, return empty set
    if (entitiesResult.isEmpty()) {
        return set<string>();
    }

    // If result from any other Clause returns no entries, return empty set
    for (RelationshipClauseResult relationshipsResult : relationshipsResults) {
        if (relationshipsResult.isEmpty()) {
            return set<string>();
        }
    }

	// Filter results to get set of entities that meet all constraints
    vector<PQLEntity> entitiesToReturn = filterEntities(entitiesResult, relationshipsResults);

	// Convert set of entities into set of entity strings
	set<string> entityStringsToReturn;

	for (PQLEntity entity : entitiesToReturn) {
		entityStringsToReturn.insert(entity.toString());
	}

	return entityStringsToReturn;
}

vector<PQLEntity> QueryEvaluator::filterEntities(EntityClauseResult entitiesResult,
											list<RelationshipClauseResult> relationshipsResults) {

	vector<vector<PQLEntity>> combinedTable;

	// vector to keep track of arguments already in combinedTable
	vector<ClauseArgument> argumentsInCombinedTable;

	// Find constraint clause with a clause argument matching select, initialise currentTable
	for (RelationshipClauseResult relationshipsResult : relationshipsResults) {
		if (entitiesResult.getArg() == relationshipsResult.getFirstArg() || entitiesResult.getArg() == relationshipsResult.getSecondArg()) {

			// Key or value does not matter at this point, arbitrarily set first column to be key
			vector<vector<PQLEntity>> tableToAdd = this->getTable(relationshipsResult);
			combinedTable = tableToAdd;

			// Add clause arguments from this RelationshipClauseResult to vector for keeping track of columns in combined table
			argumentsInCombinedTable.push_back(relationshipsResult.getFirstArg());
			argumentsInCombinedTable.push_back(relationshipsResult.getSecondArg());

			// Remove since it has been added to the combined table
			relationshipsResults.remove(relationshipsResult);

			break;
		}
	}

	// WRONG
	// If no constraint clauses have matching clause argument to select, treat all as boolean and just return selected entity list
	if (combinedTable.empty()) {
		return entitiesResult.getEntities();
	}

	// TODO: Definitely some room for optimisation here
	// Iterate through list of RelationshipClauseResults: if result has matching clause argument(s), perform a table join, if not skip
	// Repeat until empty or no more results can be table joined (i.e. the remaining ones have arguments independent of all the others, so treat as boolean)
	combinedTableJoin(&combinedTable, &argumentsInCombinedTable, &relationshipsResults);

	// From combined table, pull out column of entities that match select clause argument
	vector<PQLEntity> entitiesToReturn;

	for (int i = 0; i < argumentsInCombinedTable.size(); i++) {
		if (argumentsInCombinedTable[i] == entitiesResult.getArg()) {
			for (vector<PQLEntity> row : combinedTable) {
				entitiesToReturn.push_back(row[i]);
			}
			break;
		}
	}

	return entitiesToReturn;
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
		} else {
			output.push_back(vector<PQLEntity>{relationship.getSecondEntity(), relationship.getFirstEntity()});
		}
	}

	return output;
}

int QueryEvaluator::findArgumentIndex(vector<ClauseArgument> argumentsInTable, ClauseArgument argToFind) {

	int argIndex = -1;

	for (int i = 0; i < argumentsInTable.size(); i++) {
		if (argumentsInTable[i] == argToFind) {
			argIndex = i;
			break;
		}
	}

	return argIndex;

}

vector<pair<PQLEntity, vector<PQLEntity>>> QueryEvaluator::convertToKeyValuePairs(vector<vector<PQLEntity>> table, int key) {
	vector<pair<PQLEntity, vector<PQLEntity>>> keyValuePairs;
	for (vector<PQLEntity> row : table) {
		PQLEntity keyEntity = row[key];
		keyValuePairs.emplace_back(keyEntity, row);
	}
	return keyValuePairs;
}

vector<pair<vector<PQLEntity>, vector<PQLEntity>>> QueryEvaluator::convertToKeyValuePairs(vector<vector<PQLEntity>> table, int firstKey, int secondKey) {
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
		vector<vector<PQLEntity>> tableToMerge) {

	// Create multimap
	multimap<vector<PQLEntity>, vector<PQLEntity>> combinedTableMap;

	// Add combinedTable key-value pairs to multimap
	for (pair<vector<PQLEntity>, vector<PQLEntity>> keyValuePair : combinedTableKeyValuePairs) {
		combinedTableMap.insert(keyValuePair);
	}

	// Create output table
	vector<vector<PQLEntity>> output;

	for (vector<PQLEntity> row : tableToMerge) {
		// For each key in the tableToMerge, find set of values with this same key in combinedTable (multimap uses a range)
		pair<multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator, multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator> range;
		range = combinedTableMap.equal_range(row);

		// Each value in range is an entry in new table, so add to new table
		for (multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			output.push_back(mapIter->second);
		}
	}
	return output;
}

vector<vector<PQLEntity>> QueryEvaluator::singleKeyTableJoin(
		vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs,
		vector<vector<PQLEntity>> tableToMerge) {

	// Create multimap
	multimap<PQLEntity, vector<PQLEntity>> combinedTableMap;

	// Add combinedTable key-value pairs to multimap
	for (pair<PQLEntity, vector<PQLEntity>> keyValuePair : combinedTableKeyValuePairs) {
		combinedTableMap.insert(keyValuePair);
	}

	// Create output table
	vector<vector<PQLEntity>> output;

	for (vector<PQLEntity> row : tableToMerge) {
		// For each key in the tableToMerge, find set of values with this same key in combinedTable (multimap uses a range)
		pair<multimap<PQLEntity, vector<PQLEntity>>::iterator, multimap<PQLEntity, vector<PQLEntity>>::iterator> range;
		range = combinedTableMap.equal_range(row[0]);

		// Each value in range combined with corresponding value in tableToMerge is an entry in new table, so add to new table
		for (multimap<PQLEntity, vector<PQLEntity>>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			(mapIter->second).push_back(row[1]);
			output.push_back(mapIter->second);
		}
	}
	return output;
}

void QueryEvaluator::combinedTableJoin(vector<vector<PQLEntity>>* combinedTable,
									   vector<ClauseArgument>* argumentsInCombinedTable,
									   list<RelationshipClauseResult>* relationshipsResults) {

	list<RelationshipClauseResult> resultsLeft = *relationshipsResults;
	while (!resultsLeft.empty()) {

		list<RelationshipClauseResult> resultsSkipped;

		list<RelationshipClauseResult>::iterator resultIter = resultsLeft.begin();
		for (; resultIter != resultsLeft.end(); resultIter++) {

			// If equals -1, means ClauseArguments not yet in combined table
			int firstArgIndex = this->findArgumentIndex(*argumentsInCombinedTable, resultIter->getFirstArg());
			int secondArgIndex = this->findArgumentIndex(*argumentsInCombinedTable, resultIter->getSecondArg());

			// Neither clause argument already in currentTable so cannot join, so skip
			if (firstArgIndex == -1 && secondArgIndex == -1) {
				resultsSkipped.push_back(*resultIter);
				continue;
			}

			// Create new table
			vector<vector<PQLEntity>> newCombinedTable;

			// Both clause arguments already in table, table join on these common clause arguments
			if (firstArgIndex != -1 && secondArgIndex != -1) {
				vector<pair<vector<PQLEntity>, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
						*combinedTable, firstArgIndex, secondArgIndex);

				vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getTable(*resultIter);

				newCombinedTable = this->pairKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
			}

				// First clause argument already in table, table join on this common clause argument
			else if (firstArgIndex != -1) {
				vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
						*combinedTable, firstArgIndex);

				vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getKeyValueTable(
						*resultIter, KeyColumn::FIRST_COLUMN_KEY);

				newCombinedTable = this->singleKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
				argumentsInCombinedTable->push_back(resultIter->getSecondArg());
			}

				// Second clause argument already in table, table join on this common clause argument
			else {
				vector<pair<PQLEntity, vector<PQLEntity>>> combinedTableKeyValuePairs = this->convertToKeyValuePairs(
						*combinedTable, secondArgIndex);

				vector<vector<PQLEntity>> tableToMergeKeyValuePairs = this->getKeyValueTable(
						*resultIter, KeyColumn::SECOND_COLUMN_KEY);

				newCombinedTable = this->singleKeyTableJoin(combinedTableKeyValuePairs, tableToMergeKeyValuePairs);
				argumentsInCombinedTable->push_back(resultIter->getFirstArg());
			}

			// Set new table
			combinedTable = &newCombinedTable;

		}

		// If with one iteration of the for loop, the remaining results have not been joined with combinedTable, treat them as boolean
		if (resultsLeft == resultsSkipped) {
			if (resultsLeft.size() < 2) {
				break;
			}
		}
		resultsLeft = resultsSkipped;
	}
}

