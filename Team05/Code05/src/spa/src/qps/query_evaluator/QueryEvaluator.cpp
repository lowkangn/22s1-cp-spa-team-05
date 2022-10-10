#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
	list<ClauseResult> entitiesResults = dereferenceEntityResults(query.executeSelect(pkb));
	list<ClauseResult> relationshipsResults = dereferenceRelationshipResults(query.executeSuchThatAndPattern(pkb));

	// Empty lists indicate `Select BOOLEAN` with no constraint clauses, in which case just return TRUE
	if (entitiesResults.empty() && relationshipsResults.empty()) {
		return set<string>{"TRUE"};
	}

	if (relationshipsResults.empty()) {
		Table selectSynonymsCrossProductTable = this->getSelectSynonymsCrossProductTable(entitiesResults);
		return this->convertTableToString(selectSynonymsCrossProductTable, false);
	}

	// Combine constraint clauses - if returns empty, then no query matches
	ClauseResult combinedResult = this->combineResults(relationshipsResults);
	if (combinedResult.isEmpty()) {
		return this->convertTableToString({}, query.checkIfBooleanReturnType());
	}

	// Get a table of entries matching combination specified in select clause
	Table desiredSynonymsTable = this->getDesiredSynonymsTable(entitiesResults, combinedResult);

	// Convert results to strings
	return this->convertTableToString(desiredSynonymsTable, query.checkIfBooleanReturnType());
}

ClauseResult QueryEvaluator::combineResults(list<ClauseResult> results) {
	// TODO: Do a little optimisation here x3

	// Iterate and combine results
	ClauseResult combinedResult;
	for (list<ClauseResult>::iterator resultIter = results.begin(); resultIter != results.end(); resultIter++) {
		if (resultIter == results.begin()) {
			combinedResult = *resultIter;
		} else {
			ClauseResult newCombinedResult = combinedResult.mergeResult(*resultIter);
			combinedResult = newCombinedResult;
		}
	}

	return combinedResult;
}

Table QueryEvaluator::getDesiredSynonymsTable(list<ClauseResult> selectResultsList, ClauseResult result) {
	// If no select results, we are returning boolean, so just return table
	if (selectResultsList.empty()) {
		return result.getTable();
	}

	// Convert to vector
	vector<ClauseResult> selectResults;
	selectResults.insert(selectResults.end(), selectResultsList.begin(), selectResultsList.end());

	// Get list of args from select results
	vector<ClauseArgument> selectArgs;
	for (ClauseResult selectResult : selectResults) {
		vector<ClauseArgument> args = selectResult.getArgs();
		selectArgs.insert(selectArgs.end(), args.begin(), args.end());
	}

	// Check if any select args in combined table - if not, do cross product with select results
	bool areSelectArgsInCombinedTable = this->checkSelectArgsInCombinedTable(selectArgs, result);
	if (!areSelectArgsInCombinedTable) {
		return this->getSelectSynonymsCrossProductTable(selectResultsList);
	}

	// Find corresponding column indices
	vector<int> desiredSynonymIndices = result.getColumnIndices(selectArgs);

	// If arg not found in combined table, merge respective list from select result
	for (int i = 0; i < desiredSynonymIndices.size(); i++) {
		if (desiredSynonymIndices[i] == -1) {
			result = result.mergeResult(selectResults[i]);
			desiredSynonymIndices[i] = int(result.getArgs().size()) - 1;
		}
	}

	// If all selected args can be found in combined table, get table of desired synonyms
	Table desiredSynonymsTable;
	for (Row row : result.getTable()) {
		Row newRow;
		for (int index : desiredSynonymIndices) {
			newRow.push_back(row[index]);
		}
		desiredSynonymsTable.push_back(newRow);
	}

	return desiredSynonymsTable;
}

Table QueryEvaluator::getSelectSynonymsCrossProductTable(list<ClauseResult> selectResultsList) {
	// Convert to vector
	vector<ClauseResult> selectResults;
	selectResults.insert(selectResults.end(), selectResultsList.begin(), selectResultsList.end());

	// Make map to check if arguments already in table: if so, just copy the column, else do a cross product
	unordered_map<ClauseArgument, int> argMap;
	ClauseResult combinedResult;

	for (int i = 0; i < selectResults.size(); i++) {
		ClauseResult currentResult = selectResults[i];
		if (combinedResult.isEmpty()) {
			combinedResult = currentResult;
		} else {
			// TODO: Make this neater: make use of the fact that currentResult is EntityClauseResult
			unordered_map<ClauseArgument, int>::iterator mapIter = argMap.find(currentResult.getArgs()[0]);
			if (mapIter == argMap.end()) {
				combinedResult = combinedResult.mergeResult(currentResult);
			} else {
				ClauseResult columnToAdd = combinedResult.getColumn(mapIter->second);
				combinedResult.addColumn(columnToAdd);
			}
		}
		argMap.insert({currentResult.getArgs()[0], i});
	}

	return combinedResult.getTable();
}

set<string> QueryEvaluator::convertTableToString(Table desiredSynonymsTable, bool isBooleanReturnType) {
	set<string> entityStringsToReturn;

	if (isBooleanReturnType) {
		if (desiredSynonymsTable.empty()) {
			entityStringsToReturn.insert("FALSE");
		} else {
			entityStringsToReturn.insert("TRUE");
		}
	}
	else {
		for (Row row: desiredSynonymsTable) {
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
