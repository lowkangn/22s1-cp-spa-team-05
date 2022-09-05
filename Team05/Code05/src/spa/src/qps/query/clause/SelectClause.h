#pragma once

#include <qps/query/clause/Clause.h>
#include <qps/query/clause/ClauseArgument.h>

class SelectClause : public Clause {
private:
	ClauseArgument toSelect;
public:
	SelectClause(ClauseArgument toSelect) : toSelect(toSelect) {};
	ClauseResult execute() override { return ClauseResult(); };
	bool equals(Clause* other) override;
};