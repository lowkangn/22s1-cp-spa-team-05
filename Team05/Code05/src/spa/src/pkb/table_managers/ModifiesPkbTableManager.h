#pragma once

#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <unordered_map>
#include <string>
#include <vector>

class ModifiesPkbTableManager : public RelationshipPkbTableManager {
public:
	ModifiesPkbTableManager(EntityPkbTableManager entityManager) {
		this->entityManager = entityManager;
	}

	vector<int> filter(PkbClause clause) override;

	int add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<int, PkbRelationship> mappings;
};
