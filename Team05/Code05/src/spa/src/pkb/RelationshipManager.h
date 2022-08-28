#pragma once
#include "../dataclasses/Entity.h"
#include "../dataclasses/QueryClause.h"
#include "../dataclasses/Relationship.h"
#include "FollowsManager.h"
#include "FollowsTManager.h"
#include "Manager.h"
#include "ModifiesManager.h"
#include "ParentManager.h"
#include "ParentTManager.h"
#include "UsesManager.h"
#include <map>

using namespace std;

class RelationshipManager: public Manager {
public:
	std::string filter(QueryClause queryClause) override;

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
