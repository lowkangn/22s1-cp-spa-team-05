#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <string>
#include <vector>


vector<int> ModifiesPkbTableManager::filter(PkbClause clause) {
	vector<int> results = vector<int>();

	// If the clause lacks a LHS, it must have a RHS. Otherwise it would be invalid.
	if (clause.getLhs() == "") {
		int variable = entityManager.findVariable(clause.getRhs());

		for (auto pair : mappings) {
			if (pair.second.getRhs() == variable) {
				results.push_back(pair.second.getLhs());
			}
		}
	}
	else {
		int statement = entityManager.findStatement(stoi(clause.getLhs()));

		if (!statement) {
			return results;
		}

		for (auto pair : mappings) {
			if (pair.second.getLhs() == statement) {
				results.push_back(pair.second.getRhs());
			}
		}
	}
	return results;
}

int ModifiesPkbTableManager::add(PkbRelationship relationship) {
	mappings.insert({ nextId, relationship });
	nextId++;
	return nextId - 1;
}

void ModifiesPkbTableManager::clearDataBase() {
	mappings.clear();
	nextId = 0;
}
