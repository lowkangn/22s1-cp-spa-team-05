#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <string>
#include <vector>


vector<int> ModifiesPkbTableManager::filter(PkbClause clause) {
	vector<int> results = vector<int>();
	const string EMPTY_STRING = "";

	if (clause.getLhs() == EMPTY_STRING) {

		string varString = clause.getRhs();
		int varId = this->entityManager.findVariableByString(varString);
		
		for (auto idAndRship : this->idToRshipMapping) {

			PkbRelationship relationship = idAndRship.second;
			int modifier = relationship.getLhs();
			int modified = relationship.getRhs();

			// Find the statement that modify this variable.
			if (modified == varId) {
				results.push_back(modifier);
			}
		}
	}
	else {
		int lineNumber = stoi(clause.getLhs());
		int statementId = this->entityManager.findStatement(lineNumber);

		if (!statementId) {
			return results;
		}

		for (auto idAndRship : this->idToRshipMapping) {

			PkbRelationship relationship = idAndRship.second;
			int modifier = relationship.getLhs();
			int modified = relationship.getRhs();

			// Find the variable modified by this statement.
			if (modifier == statementId) {
				results.push_back(modified);
			}
		}
	}
	return results;
}

int ModifiesPkbTableManager::add(PkbRelationship relationship) {
	this->idToRshipMapping.insert({ this->nextId, relationship });
	this->nextId++;
	return this->nextId - 1;
}

void ModifiesPkbTableManager::clearDataBase() {
	this->idToRshipMapping.clear();
	this->nextId = 0;
}

// Added for PKB testing
unordered_map<int, PkbRelationship> ModifiesPkbTableManager::getMap() {
	return this->idToRshipMapping;
}
