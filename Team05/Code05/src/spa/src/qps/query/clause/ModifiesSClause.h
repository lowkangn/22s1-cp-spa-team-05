#pragma once

#include <string>
#include "Clause.h"
#include "ClauseResult.h"
#include "RelationshipClauseResult.h"
#include "ClauseArgument.h"

class ModifiesSClause : public Clause {
private:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	ModifiesSClause(ClauseArgument lhs, ClauseArgument rhs)
		: lhs(lhs), rhs(rhs) {};

	shared_ptr<ClauseResult> execute() override;

	bool equals(const Clause* other) override;
};
