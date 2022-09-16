#pragma once

#include <qps/query/clause/RelationshipClause.h>

class ParentClause : public RelationshipClause {
public:
    ParentClause(ClauseArgument lhs, ClauseArgument rhs)
        : RelationshipClause(lhs, rhs) {};

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

    bool equals(const RelationshipClause* other) override;
};
