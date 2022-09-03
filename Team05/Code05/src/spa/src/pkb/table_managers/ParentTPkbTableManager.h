#pragma once

#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <unordered_map>
#include <vector>
#include <string>

class ParentTPkbTableManager : public RelationshipPkbTableManager{
public:
	string filter(PkbClause clause) override;

	void add(PkbRelationship relationship) override;

	void clearDataBase() override;

private:
	unordered_map<int, vector<PkbRelationship>> mappings;
};
