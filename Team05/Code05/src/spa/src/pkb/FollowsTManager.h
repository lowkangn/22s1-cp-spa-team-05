#pragma once
#include "../dataclasses/Entity.h"
#include "../dataclasses/QueryClause.h"
#include "../dataclasses/Relationship.h"
#include "Manager.h"
#include <map>
#include <string>
#include <vector>

class FollowsTManager : public Manager {
public:
	std::string filter(QueryClause queryClause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	std::map<Entity, std::vector<Relationship>> mappings;
};
