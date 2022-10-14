#pragma once

#include <qps/query/clause/RelationshipClause.h>

class CallsTClause : public RelationshipClause {
public:
    CallsTClause(ClauseArgument lhs, ClauseArgument rhs)
        : RelationshipClause(lhs, rhs) {};

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;
    bool equals(shared_ptr<RelationshipClause> other) override;
};
