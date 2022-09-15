#pragma once

#include <qps/query/clause/RelationshipClause.h>

class FollowsTClause : public RelationshipClause {
public:
    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
