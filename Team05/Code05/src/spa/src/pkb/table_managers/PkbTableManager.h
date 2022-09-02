#pragma once

#include <pkb/design_objects/PkbClause.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <string>

using namespace std;

class PkbTableManager {
public:
	virtual string filter(PkbClause clause) = 0;
	
	virtual void add(Relationship relationship) = 0;

	virtual void clearDataBase() = 0;
};
