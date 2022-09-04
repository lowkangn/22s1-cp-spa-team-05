#pragma once

#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <vector>
#include <string>

class PatternPkbTableManager : public RelationshipPkbTableManager {
public:
    PatternPkbTableManager(EntityPkbTableManager& entityManager) : RelationshipPkbTableManager(entityManager) {}

    vector<int> filter(PkbClause clause) override;

	void add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	// Change to vector<Patterns> once class is implemented
	vector<int> patterns;
};
