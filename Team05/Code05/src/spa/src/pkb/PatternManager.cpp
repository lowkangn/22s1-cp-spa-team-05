#include <string>
#include "Manager.h"
#include <vector>

class PatternManager : public Manager {

	// Change to vector<Patterns> once class is implemented
	std::vector<int> patterns;

	//TODO Change from string to QueryClause class
	std::string filter(std::string queryClause) override {
		return std::string(0);
	}

	//TODO Change from string to Relationship class
	void add(std::string relationship) override {

	}

	void clearDataBase() override {

	}
};