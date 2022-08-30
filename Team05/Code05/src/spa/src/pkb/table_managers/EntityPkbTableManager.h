#pragma once

#include <PkbTableManager.h>
#include <vector>
#include <string>

class EntityPkbTableManager : public PkbTableManager {
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	// Change to the vector<Variables> once variables is implemented
	std::vector<int> variables;
	// Change to the vector<Statements> once variables is implemented
	std::vector<int> statements;
};
