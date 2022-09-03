#include <pkb/table_managers/EntityPkbTableManager.h>
#include <string>
#include <vector>

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
		idToEntityMapping.at(nextId) = entity;
		nextId++;
	}
	else {
		return entityToIdMapping[entity];
	}
	return 0;
}

void EntityPkbTableManager::clearDataBase() {
	idToEntityMapping.clear();
	entityToIdMapping.clear();
}
