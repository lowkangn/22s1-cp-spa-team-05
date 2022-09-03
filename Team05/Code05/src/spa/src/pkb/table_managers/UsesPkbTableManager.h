#pragma once

#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <unordered_map>
#include <string>

class UsesPkbTableManager : public RelationshipPkbTableManager {
public:
	string filter(PkbClause) override;

	void add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<int, PkbRelationship> mappings;
};
