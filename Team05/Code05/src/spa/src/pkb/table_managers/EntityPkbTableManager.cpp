#include <pkb/table_managers/EntityPkbTableManager.h>
#include <string>
#include <vector>

vector<PkbEntity> EntityPkbTableManager::filter(vector<int> ids)  {
	return PkbEntity{ PkbEntity(1, Token{ })};
}

void EntityPkbTableManager::add(Entity entity) {
	if (entityToIdMapping.find(entity) == entityToIdMapping.end()) {

		entityToIdMapping[Entity] = nextId;
		idToEntityMapping[nextId] = Entity(Token{ });
		nextId++;
	}
	else {
		return entityToIdMapping[Entity];
	}
}

void EntityPkbTableManager::clearDataBase() {
	idToEntityMapping.clear();
	entityToIdMapping.clear();
}
