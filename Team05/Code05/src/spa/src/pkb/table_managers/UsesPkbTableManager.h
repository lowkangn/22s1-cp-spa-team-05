#pragma once

#include <PkbTableManager.h>
#include <map>
#include <string>

class UsesPkbTableManager : public PkbTableManager {
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	std::map<Entity, Relationship> mappings;
};
