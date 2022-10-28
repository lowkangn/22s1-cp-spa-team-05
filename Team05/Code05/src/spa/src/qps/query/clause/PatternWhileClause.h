#pragma once

#include <string>
#include <qps/query/clause/PatternClause.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/clause/ClauseArgument.h>

class PatternWhileClause : public PatternClause {
protected:
	PKBTrackedStatementType getStatementType() override;
public:
	PatternWhileClause(ClauseArgument patternArg, ClauseArgument lhs, ClauseArgument rhs)
		: PatternClause(patternArg, lhs, rhs) {};

	bool equals(shared_ptr<PatternClause> other) override;
};
