#pragma once

#include <qps/query/clause/RelationshipClause.h>

class ParentTClause : public RelationshipClause {
public:
    ParentTClause(ClauseArgument lhs, ClauseArgument rhs)
        : RelationshipClause(lhs, rhs) {};

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;
    bool equals(const RelationshipClause* other) override;
};
