#pragma once

#include <string>
#include "SuchThatClause.h"
#include "RelationshipClauseResult.h"
#include "ClauseArgument.h"

class ModifiesSClause : public SuchThatClause {
public:
	ModifiesSClause(ClauseArgument lhs, ClauseArgument rhs)
		: SuchThatClause(lhs, rhs) {};

	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;

	bool equals(const SuchThatClause* other) override;
};
