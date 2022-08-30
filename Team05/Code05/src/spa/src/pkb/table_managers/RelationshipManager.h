#pragma once

#include <FollowsManager.h>
#include <FollowsTManager.h>
#include <Manager.h>
#include <ModifiesManager.h>
#include <ParentManager.h>
#include <ParentTManager.h>
#include <UsesManager.h>
#include <map>
#include <string>

using namespace std;

class RelationshipManager: public Manager {
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	FollowsManager followsManager;
	FollowsTManager followsTManager;
	ModifiesManager modifiesManager;
	ParentManager parentManager;
	ParentTManager parentTManager;
	UsesManager usesManager;
};
