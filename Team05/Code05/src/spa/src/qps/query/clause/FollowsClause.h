#pragma once

#include "SuchThatClause.h"

class FollowsClause : public SuchThatClause {
public:
    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
