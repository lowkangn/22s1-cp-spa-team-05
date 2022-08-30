#pragma once

#include <PkbTableManager.h>
#include <map>
#include <string>
#include <vector>

class FollowsTPkbTableManager : public PkbTableManager {
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	std::map<Entity, std::vector<Relationship>> mappings;
};
