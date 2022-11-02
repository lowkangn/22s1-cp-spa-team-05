#pragma once

#include <qps/query/clause/RelationshipClause.h>

class NextTClause : public RelationshipClause {
public:
    NextTClause(ClauseArgument lhs, ClauseArgument rhs)
        : RelationshipClause(lhs, rhs) {};

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

    bool equals(shared_ptr<RelationshipClause> other) override;

    bool requiresCfg() override {
        return true;
    }
};
