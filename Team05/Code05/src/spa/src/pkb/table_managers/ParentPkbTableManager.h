#pragma once

#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <unordered_map>
#include <string>

class ParentPkbTableManager : public RealtionshipPkbTableManager {
public:
    ParentPkbTableManager(EntityPkbTableManager& entityManager) : RelationshipPkbTableManager(entityManager) {}

    vector<int> filter(PkbClause clause) override;

	void add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<int, PkbRelationship> mappings;
};
