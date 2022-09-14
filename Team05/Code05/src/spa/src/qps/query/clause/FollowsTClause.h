#pragma once

#include "SuchThatClause.h"

class FollowsTClause : public SuchThatClause {
public:
    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
