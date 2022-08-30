#pragma once

#include <PkbTableManager.h>
#include <map>
#include <vector>
#include <string>

class ParentTPkbTableManager : public PkbTableManager{
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	std::map<Entity, std::vector<Relationship>> mappings;
};
