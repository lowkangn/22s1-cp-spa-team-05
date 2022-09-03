#pragma once

#include <pkb/design_objects/PkbEntity.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <vector>
#include <string>
#include <unordered_map>

class EntityPkbTableManager : public PkbTableManager {

public:
	vector<PkbEntity> filter(vector<int> ids) override;

	int add(Entity entity);

	void clearDataBase() override;

private:
	unordered_map<int, PkbEntity> idToEntityMapping;
	unordered_map<Entity, int> entityToIdMapping;
	int nextId = 0;
};
