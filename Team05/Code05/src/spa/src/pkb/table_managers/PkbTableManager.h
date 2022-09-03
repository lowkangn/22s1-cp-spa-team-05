#pragma once

#include <pkb/design_objects/PkbClause.h>
#include <string>

using namespace std;

class PkbTableManager {
public:
	virtual void clearDataBase() = 0;
};
