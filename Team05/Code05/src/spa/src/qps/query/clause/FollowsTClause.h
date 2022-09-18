#pragma once

#include <qps/query/clause/RelationshipClause.h>

class FollowsTClause : public RelationshipClause {
public:
    FollowsTClause(ClauseArgument lhs, ClauseArgument rhs)
        : RelationshipClause(lhs, rhs) {};

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;
    bool equals(shared_ptr<RelationshipClause> other) override;
};
