#pragma once

#include <FollowsPkbTableManager.h>
#include <FollowsTPkbTableManager.h>
#include <PkbTableManager.h>
#include <ModifiesPkbTableManager.h>
#include <ParentPkbTableManager.h>
#include <ParentTPkbTableManager.h>
#include <UsesPkbTableManager.h>
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
