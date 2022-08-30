#pragma once

#include <Manager.h>
#include <vector>
#include <string>

class PatternManager : public Manager {
public:
	std::string filter(PkbQuery query) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	// Change to vector<Patterns> once class is implemented
	std::vector<int> patterns;
};
