#pragma once

#include <string>
#include "SuchThatClause.h"
#include "RelationshipClauseResult.h"
#include "ClauseArgument.h"

class ModifiesPClause : public SuchThatClause {

public:
	ModifiesPClause(ClauseArgument lhs, ClauseArgument rhs)
		: SuchThatClause(lhs, rhs) {};

	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;

	bool equals(const SuchThatClause* other) override;
};
