#pragma once

#include <Manager.h>
#include <vector>
#include <string>

class EntityManager : public Manager {
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
