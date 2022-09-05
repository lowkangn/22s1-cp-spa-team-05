#pragma once

#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <unordered_map>
#include <string>

class UsesPkbTableManager : public RelationshipPkbTableManager {
public:
    UsesPkbTableManager(EntityPkbTableManager& entityManager) : RelationshipPkbTableManager(entityManager) {}

    vector<int> filter(PkbClause clause) override;

	int add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<int, PkbRelationship> mappings;
};
