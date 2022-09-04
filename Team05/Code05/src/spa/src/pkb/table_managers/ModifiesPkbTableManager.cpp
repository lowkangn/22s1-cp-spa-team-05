#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <string>
#include <vector>

vector<int> ModifiesPkbTableManager::filter(PkbClause clause) {
	int statement = entityManager.findStatement(stoi(clause.getLhs()));
	vector<int> results = vector<int>();

	if (!statement) {
		return results;
	}

	for (auto pair : mappings) {
		if (pair.second.getLhs() == statement) {
			results.push_back(pair.second.getRhs());
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
