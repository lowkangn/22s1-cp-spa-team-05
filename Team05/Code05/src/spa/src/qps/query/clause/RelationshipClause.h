#pragma once

#include <string>
#include "ClauseArgument.h"
#include "RelationshipClauseResult.h"
#include <pkb/PKB.h>

using namespace std;

class RelationshipClause {
protected:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	RelationshipClause(ClauseArgument lhs, ClauseArgument rhs) : lhs(lhs), rhs(rhs) {};
	virtual shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) = 0;
	virtual bool equals(const RelationshipClause* other) = 0;
};
