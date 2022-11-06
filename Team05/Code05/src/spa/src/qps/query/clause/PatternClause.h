#pragma once

#include <pkb/PKB.h>

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/RelationshipClauseResult.h>

#include <memory>
#include <vector>

using namespace std;

class PatternClause {
protected:
    ClauseArgument patternArg;
    ClauseArgument lhs;
    ClauseArgument rhs;

    virtual PKBTrackedStatementType getStatementType() = 0;

public:
    PatternClause(
        ClauseArgument patternArg, ClauseArgument lhs, ClauseArgument rhs) :
        patternArg(patternArg), lhs(lhs), rhs(rhs) {
    }

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb);

    virtual bool equals(shared_ptr<PatternClause> other) = 0;
};
