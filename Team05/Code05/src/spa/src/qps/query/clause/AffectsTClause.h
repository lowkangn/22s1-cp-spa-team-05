#pragma once

#include <qps/query/clause/CfgRelationshipClause.h>

class AffectsTClause : public CfgRelationshipClause {
protected:
    PKBTrackedRelationshipType getPkbTrackedRelationshipType() override;
public:
    AffectsTClause(ClauseArgument lhs, ClauseArgument rhs)
        : CfgRelationshipClause(lhs, rhs) {}

    void acceptClauseOptimiser(CfgClauseOptimiser* optimiser) override;

    bool equals(shared_ptr<RelationshipClause> other) override;

    bool isAlwaysEmpty() override;
};
