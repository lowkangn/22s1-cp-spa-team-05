#pragma once

#include <pkb/design_objects/PkbRelationship.h>
#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <unordered_map>
#include <string>
#include <vector>

class FollowsTPkbTableManager : public RelationshipPkbTableManager {
public:
    FollowsTPkbTableManager(EntityPkbTableManager& entityManager) : RelationshipPkbTableManager(entityManager) {}

    vector<int> filter(PkbClause clause) override;

	void add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<int, vector<PkbRelationship>> mappings;
};
