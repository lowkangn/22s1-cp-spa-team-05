#pragma once

#include <string>
#include "ClauseArgument.h"
#include "EntityClauseResult.h"

using namespace std;

class SelectClause {
private:
	ClauseArgument toSelect;
public:
	SelectClause(ClauseArgument toSelect) : toSelect(toSelect) {};
	shared_ptr<EntityClauseResult> execute();
	bool equals(const SelectClause* other);
};
