#pragma once

#include <pkb/table_managers/PkbTableManager.h>
#include <unordered_map>
#include <string>

class UsesPkbTableManager : public PkbTableManager {
public:
	string filter(PkbClause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<Entity, Relationship> mappings;
};
