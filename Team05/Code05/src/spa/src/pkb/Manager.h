#pragma once
#include "../dataclasses/QueryClause.h"
#include "../dataclasses/Relationship.h"
#include <string>

class Manager {
public:
	virtual std::string filter(QueryClause queryClause) = 0;
	
	virtual void add(Relationship relationship) = 0;

	virtual void clearDataBase() = 0;
};
