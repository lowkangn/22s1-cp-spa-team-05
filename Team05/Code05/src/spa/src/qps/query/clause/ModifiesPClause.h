#pragma once

#include <string>
#include "Clause.h"
#include "ClauseResult.h"
#include "RelationshipClauseResult.h"
#include "ClauseArgument.h"

class ModifiesPClause : public Clause {

private:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	ModifiesPClause(ClauseArgument lhs, ClauseArgument rhs)
		: lhs(lhs), rhs(rhs) {};

	shared_ptr<ClauseResult> execute() override;

	bool equals(const Clause* other) override;
};