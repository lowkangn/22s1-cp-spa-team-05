#pragma once

#include <string>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/clause/ClauseArgument.h>

class ModifiesSClause : public RelationshipClause {
public:
	ModifiesSClause(ClauseArgument lhs, ClauseArgument rhs)
		: RelationshipClause(lhs, rhs) {};

	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;

	bool equals(const RelationshipClause* other) override;
};
