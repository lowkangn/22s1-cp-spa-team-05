#pragma once

#include <string>
#include "ClauseArgument.h"
#include "RelationshipClauseResult.h"

using namespace std;

class SuchThatClause {
protected:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	SuchThatClause(ClauseArgument lhs, ClauseArgument rhs) : lhs(lhs), rhs(rhs) {};
	virtual shared_ptr<RelationshipClauseResult> execute() = 0;
	virtual bool equals(const SuchThatClause* other) = 0;
};
