#include <pkb/table_managers/EntityPkbTableManager.h>
#include <string>
#include <vector>

//If no such statement can be found, return -1;
int EntityPkbTableManager::findStatement(int lineNumber) {
	for (auto pair : idToEntityMapping) {

		PkbEntity entity = pair.second;

		if (entity.isStatement() && entity.getLineNumber() == lineNumber) {
			return pair.first;
		}
	}
	return -1;
}

vector<PkbEntity> EntityPkbTableManager::filter(vector<int> ids)  {
	vector<PkbEntity> filtered = vector<PkbEntity>();

	for (auto id : ids) {
		if (idToEntityMapping.find(id) != idToEntityMapping.end()) {
			filtered.push_back(idToEntityMapping.at(id));
		}
	}
	return filtered;
}

int EntityPkbTableManager::add(PkbEntity entity) {
	if (entityToIdMapping.find(entity) == entityToIdMapping.end()) {
		entityToIdMapping[entity] = nextId;
		idToEntityMapping.insert({ nextId, entity });
		nextId++;
		return nextId - 1;
	}	
	else {
		return entityToIdMapping[entity];
	}
}

void EntityPkbTableManager::clearDataBase() {
	idToEntityMapping.clear();
	entityToIdMapping.clear();
	nextId = 0;
}
