#pragma once

#include <pkb/table_managers/PkbTableManager.h>
#include <vector>
#include <string>

class PatternPkbTableManager : public PkbTableManager {
public:
	std::string filter(PkbClause clause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	// Change to vector<Patterns> once class is implemented
	std::vector<int> patterns;
};
