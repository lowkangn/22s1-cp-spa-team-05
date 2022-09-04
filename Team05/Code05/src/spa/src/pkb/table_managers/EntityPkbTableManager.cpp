#include <pkb/table_managers/EntityPkbTableManager.h>
#include <string>
#include <vector>

//If no such variable can be found, return -1;
int EntityPkbTableManager::findVariableByString(string stringToMatch) {
	for (auto idAndEntity : this->idToEntityMapping) {

		PkbEntity entity = idAndEntity.second;

		if (entity.toString() == stringToMatch) {
			return idAndEntity.first;
		}
	}
	return -1;
}

//If no such statement can be found, return -1;
int EntityPkbTableManager::findStatement(int lineNumber) {
	for (auto idAndEntity : this->idToEntityMapping) {

		PkbEntity entity = idAndEntity.second;

		if (entity.isStatement() && entity.getLineNumber() == lineNumber) {
			return idAndEntity.first;
		}
	}
	return -1;
}

vector<PkbEntity> EntityPkbTableManager::filter(vector<int> ids)  {
	vector<PkbEntity> filtered = vector<PkbEntity>();

	for (auto id : ids) {
		if (this->idToEntityMapping.find(id) != this->idToEntityMapping.end()) {
			filtered.push_back(this->idToEntityMapping.at(id));
		}
	}
	return filtered;
}

int EntityPkbTableManager::add(PkbEntity entity) {
	if (this->entityToIdMapping.find(entity) == this->entityToIdMapping.end()) {
		this->entityToIdMapping[entity] = this->nextId;
		this->idToEntityMapping.insert({ this->nextId, entity });
		this->nextId++;
		return this->nextId - 1;
	}	
	else {
		return entityToIdMapping[entity];
	}
}

void EntityPkbTableManager::clearDataBase() {
	this->idToEntityMapping.clear();
	this->entityToIdMapping.clear();
	this->nextId = 0;
}
