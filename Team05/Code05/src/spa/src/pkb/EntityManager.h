#pragma once
#include "../dataclasses/QueryClause.h"
#include "../dataclasses/Relationship.h"
#include "Manager.h"
#include <vector>

class EntityManager : public Manager {
public:
	std::string filter(QueryClause queryClause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	// Change to the vector<Variables> once variables is implemented
	std::vector<int> variables;
	// Change to the vector<Statements> once variables is implemented
	std::vector<int> statements;
};
