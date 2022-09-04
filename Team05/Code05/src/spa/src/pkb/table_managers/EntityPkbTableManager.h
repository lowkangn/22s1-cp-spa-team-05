#pragma once

#include <pkb/design_objects/PkbEntity.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <vector>
#include <string>
#include <unordered_map>

class EntityPkbTableManager : public PkbTableManager<PkbEntity> {

public:
	int findVariableByString(string stringToMatch);

	int findStatement(int lineNumber);

	vector<PkbEntity> filter(vector<int> ids);

	int add(PkbEntity entity) override;

	void clearDataBase() override;

private:
	unordered_map<int, PkbEntity> idToEntityMapping;
	unordered_map<PkbEntity, int, EntityHasher> entityToIdMapping;
};
