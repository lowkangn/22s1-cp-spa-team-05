#pragma once

#include <qps/query/clause/Clause.h>
#include <qps/query/clause/ClauseArgument.h>

class ModifiesSClause : public Clause {
private:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	ModifiesSClause(ClauseArgument lhs, ClauseArgument rhs)
		: lhs(lhs), rhs(rhs) {};

	ClauseResult execute() override { return ClauseResult(); }; //defintiion for testing purposes only

	bool equals(const Clause* other) override;
};