#pragma once

#include <pkb/table_managers/PkbTableManager.h>
#include <unordered_map>
#include <string>
#include <vector>

class FollowsTPkbTableManager : public PkbTableManager {
public:
	std::string filter(PkbClause clause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	std::unordered_map<Entity, std::vector<Relationship>> mappings;
};
