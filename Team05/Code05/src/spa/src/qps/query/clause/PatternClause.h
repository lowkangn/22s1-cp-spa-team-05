#pragma once

#include <string>
#include <memory>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <pkb/PKB.h>

using namespace std;

class PatternClause {
protected:
	ClauseArgument patternArg;
	ClauseArgument lhs;
	ClauseArgument rhs;

public:
	PatternClause(ClauseArgument patternArg, ClauseArgument lhs, ClauseArgument rhs) : patternArg(patternArg), lhs(lhs), rhs(rhs) {};

	virtual shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) = 0;

	virtual bool equals(shared_ptr<PatternClause> other) = 0;
};