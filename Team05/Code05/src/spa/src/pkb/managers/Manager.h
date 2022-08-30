#pragma once
#include <string>


class Manager {
public:
	virtual std::string filter(std::string queryClause) = 0;
	
	virtual void add(std::string relationship) = 0;

	virtual void clearDataBase() = 0;
};