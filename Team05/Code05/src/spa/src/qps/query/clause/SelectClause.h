#pragma once

#include <string>
#include "Clause.h"
#include "ClauseResult.h"
#include "ClauseArgument.h"
#include "EntityClauseResult.h"

using namespace std;

class SelectClause : public Clause {
private:
	ClauseArgument toSelect;
public:
	SelectClause(ClauseArgument toSelect) : toSelect(toSelect) {};
	shared_ptr<ClauseResult> execute() override;
	bool equals(const Clause* other) override;
};
