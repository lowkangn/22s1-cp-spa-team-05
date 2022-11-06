#pragma once

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/RelationshipClauseResult.h>

#include <memory>
#include <vector>

class ModifiesSClause : public RelationshipClause {
public:
    ModifiesSClause(ClauseArgument lhs, ClauseArgument rhs) : RelationshipClause(lhs, rhs) {
    }

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

    bool equals(shared_ptr<RelationshipClause> other) override;
};
