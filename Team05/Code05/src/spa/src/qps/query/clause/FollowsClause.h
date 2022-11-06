#pragma once

#include <qps/query/clause/RelationshipClause.h>

#include <memory>
#include <vector>

class FollowsClause : public RelationshipClause {
public:
    FollowsClause(ClauseArgument lhs, ClauseArgument rhs) : RelationshipClause(lhs, rhs) {
    }

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

    bool equals(shared_ptr<RelationshipClause> other) override;
};
