#pragma once

#include <qps/query/clause/CfgRelationshipClause.h>

class NextClause : public CfgRelationshipClause {
protected:
    PKBTrackedRelationshipType NextClause::getPkbTrackedRelationshipType() override;
public:
    NextClause(ClauseArgument lhs, ClauseArgument rhs)
        : CfgRelationshipClause(lhs, rhs) {}

    bool equals(shared_ptr<RelationshipClause> other) override;
    
};
