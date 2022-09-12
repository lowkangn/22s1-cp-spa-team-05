#pragma once

#include "SuchThatClause.h"

class FollowsClause : public SuchThatClause {
public:
    shared_ptr<RelationshipClauseResult> execute() override;
};
