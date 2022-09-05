#pragma once

#include <qps/query/clause/Clause.h>

class ModifiesPClause : public Clause {

private:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	ModifiesPClause(ClauseArgument lhs, ClauseArgument rhs)
		: lhs(lhs), rhs(rhs) {};

	ClauseResult execute() override { return ClauseResult(); }; //defintiion for testing purposes only

	bool equals(Clause* other) override;
};