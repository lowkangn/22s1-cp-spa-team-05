#pragma once

#include <string>
#include "RelationshipClause.h"
#include "RelationshipClauseResult.h"
#include "ClauseArgument.h"

class ModifiesSClause : public RelationshipClause {
public:
	ModifiesSClause(ClauseArgument lhs, ClauseArgument rhs)
		: RelationshipClause(lhs, rhs) {};

	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;

	bool equals(const RelationshipClause* other) override;
};
