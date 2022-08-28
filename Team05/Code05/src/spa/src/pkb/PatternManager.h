#pragma once
#include "../dataclasses/QueryClause.h"
#include "../dataclasses/Relationship.h"
#include "Manager.h"
#include <vector>

class PatternManager : public Manager {
public:
	std::string filter(QueryClause queryClause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	// Change to vector<Patterns> once class is implemented
	std::vector<int> patterns;
};
