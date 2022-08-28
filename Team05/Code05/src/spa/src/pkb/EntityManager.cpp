#include <string>
#include "Manager.h"
#include <vector>

class EntityManager : public Manager {

	// Change to the vector<Variables> once variables is implemented
	std::vector<int> variables;
	// Change to the vector<Statements> once variables is implemented
	std::vector<int> statements;

	//TODO Change from string to QueryClause class
	std::string filter(std::string queryClause) override {
		return std::string{"0"};
	}

	//TODO Change from string to Relationship class
	void add(std::string relationship) override {

	}

	void clearDataBase() override {

	}
};