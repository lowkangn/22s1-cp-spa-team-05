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
    list<RelationshipClauseResult> relationshipsResults =
			this->dereferenceRelationshipsResultPointers(relationshipsResultPointers);

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

list<RelationshipClauseResult> QueryEvaluator::dereferenceRelationshipsResultPointers(
		list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers) {
	list<RelationshipClauseResult> relationshipsToReturn;
	for (shared_ptr<RelationshipClauseResult> relationshipsResultPointer : relationshipsResultPointers) {
		relationshipsToReturn.push_back(*relationshipsResultPointer);
	}
	return relationshipsToReturn;
}

vector<PQLEntity> QueryEvaluator::filterEntities(EntityClauseResult entitiesResult,
											list<RelationshipClauseResult> relationshipsResults) {

	vector<vector<PQLEntity>> currentTable;
	vector<ClauseArgument> argumentsInCurrentTable;

	for (RelationshipClauseResult relationshipsResult : relationshipsResults) {
		if (entitiesResult.getArg() == relationshipsResult.getFirstArg() || entitiesResult.getArg() == relationshipsResult.getSecondArg()) {
			vector<vector<PQLEntity>> tableToAdd = this->getKeyOnlyTable(relationshipsResult);
			currentTable = tableToAdd;
			argumentsInCurrentTable.push_back(relationshipsResult.getFirstArg());
			argumentsInCurrentTable.push_back(relationshipsResult.getSecondArg());
			relationshipsResults.remove(relationshipsResult);
			break;
		}
	}

	if (currentTable.empty()) {
		return entitiesResult.getEntities();
	}

	list<RelationshipClauseResult> resultsLeft = relationshipsResults;
	while (!resultsLeft.empty()) {

		list<RelationshipClauseResult> resultsSkipped;

		list<RelationshipClauseResult>::iterator resultIter = resultsLeft.begin();
		for (; resultIter != resultsLeft.end(); resultIter++) {

			// If equals -1, means ClauseArguments not yet in combined table
			int firstArgIndex = this->findArgumentIndex(argumentsInCurrentTable, resultIter->getFirstArg());
			int secondArgIndex = this->findArgumentIndex(argumentsInCurrentTable, resultIter->getSecondArg());


			if (firstArgIndex == -1 && secondArgIndex == -1) {
				resultsSkipped.push_back(*resultIter);
				continue;
			} else {
				vector<vector<PQLEntity>> combinedTable;
				if (firstArgIndex != -1 && secondArgIndex != -1) {
					vector<pair<vector<PQLEntity>, vector<PQLEntity>>> currentTableKeyValuePairs = this->convertToKeyValuePairs(
							currentTable, firstArgIndex, secondArgIndex);
					vector<vector<PQLEntity>> tableToMerge = this->getKeyOnlyTable(*resultIter);
					combinedTable = this->pairKeyTableJoin(currentTableKeyValuePairs, tableToMerge);
				} else if (firstArgIndex != -1) {
					vector<pair<PQLEntity, vector<PQLEntity>>> currentTableKeyValuePairs = this->convertToKeyValuePairs(
							combinedTable, firstArgIndex);
					vector<vector<PQLEntity>> tableToMerge = this->getKeyValueTable(*resultIter, KeyColumn::FIRST_COLUMN_KEY);
					combinedTable = this->singleKeyTableJoin(currentTableKeyValuePairs, tableToMerge);
					argumentsInCurrentTable.push_back(resultIter->getSecondArg());
				} else {
					vector<pair<PQLEntity, vector<PQLEntity>>> currentTableKeyValuePairs = this->convertToKeyValuePairs(
							combinedTable, secondArgIndex);
					vector<vector<PQLEntity>> tableToMerge = this->getKeyValueTable(*resultIter, KeyColumn::SECOND_COLUMN_KEY);
					combinedTable = this->singleKeyTableJoin(currentTableKeyValuePairs, tableToMerge);
					argumentsInCurrentTable.push_back(resultIter->getFirstArg());
				}
				currentTable = combinedTable;
			}
		}
		if (resultsLeft == resultsSkipped) {
			break;
		}
		resultsLeft = resultsSkipped;
	}

	vector<PQLEntity> entitiesToReturn;

	for (int i = 0; i < argumentsInCurrentTable.size(); i++) {
		if (argumentsInCurrentTable[i] == entitiesResult.getArg()) {
			for (vector<PQLEntity> row : currentTable) {
				entitiesToReturn.push_back(row[i]);
			}
			break;
		}
	}

	return entitiesToReturn;
}

vector<vector<PQLEntity>> QueryEvaluator::getKeyOnlyTable(RelationshipClauseResult relationshipsResult) {

	return this->getKeyValueTable(relationshipsResult, KeyColumn::FIRST_COLUMN_KEY);
}

vector<vector<PQLEntity>> QueryEvaluator::getKeyValueTable(RelationshipClauseResult relationshipsResult, KeyColumn keyColumn) {

	vector<PQLRelationship> relationships = relationshipsResult.getRelationships();
	vector<vector<PQLEntity>> output;

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
		vector<pair<vector<PQLEntity>, vector<PQLEntity>>> currentTableKeyValuePairs,
		vector<vector<PQLEntity>> tableToMerge) {

	multimap<vector<PQLEntity>, vector<PQLEntity>> combinedTableMap;
	for (pair<vector<PQLEntity>, vector<PQLEntity>> keyValuePair : currentTableKeyValuePairs) {
		combinedTableMap.insert(keyValuePair);
	}
	vector<vector<PQLEntity>> output;
	for (vector<PQLEntity> row : tableToMerge) {
		pair<multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator, multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator> range;
		range = combinedTableMap.equal_range(row);
		for (multimap<vector<PQLEntity>, vector<PQLEntity>>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			output.push_back(mapIter->second);
		}
	}
	return output;
}

vector<vector<PQLEntity>> QueryEvaluator::singleKeyTableJoin(
		vector<pair<PQLEntity, vector<PQLEntity>>> currentTableKeyValuePairs,
		vector<vector<PQLEntity>> tableToMerge) {

	multimap<PQLEntity, vector<PQLEntity>> combinedTableMap;

	// insert key value pairs from the combined table into the multimap
	for (pair<PQLEntity, vector<PQLEntity>> keyValuePair : currentTableKeyValuePairs) {
		combinedTableMap.insert(keyValuePair);
	}

	vector<vector<PQLEntity>> output;

	for (vector<PQLEntity> row : tableToMerge) {
		pair<multimap<PQLEntity, vector<PQLEntity>>::iterator, multimap<PQLEntity, vector<PQLEntity>>::iterator> range;
		range = combinedTableMap.equal_range(row[0]);
		for (multimap<PQLEntity, vector<PQLEntity>>::iterator mapIter = range.first; mapIter != range.second; mapIter++) {
			(mapIter->second).push_back(row[1]);
			output.push_back(mapIter->second);
		}
	}
	return output;
}

