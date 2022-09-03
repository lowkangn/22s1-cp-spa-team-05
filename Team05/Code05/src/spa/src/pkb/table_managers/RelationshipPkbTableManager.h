#pragma once

#include <pkb/table_managers/FollowsPkbTableManager.h>
#include <pkb/table_managers/FollowsTPkbTableManager.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <pkb/table_managers/ParentPkbTableManager.h>
#include <pkb/table_managers/ParentTPkbTableManager.h>
#include <pkb/table_managers/UsesPkbTableManager.h>
#include <map>
#include <string>

using namespace std;

class RelationshipPkbTableManager: public PkbTableManager {
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	FollowsPkbTableManager followsPkbTableManager;
	FollowsTPkbTableManager followsTPkbTableManager;
	ModifiesPkbTableManager modifiesPkbTableManager;
	ParentPkbTableManager parentPkbTableManager;
	ParentTPkbTableManager parentTPkbTableManager;
	UsesPkbTableManager usesPkbTableManager;
};
