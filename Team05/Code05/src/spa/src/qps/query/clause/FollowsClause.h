#pragma once

#include "Clause.h"

class FollowsClause : public Clause {
public:
    shared_ptr<ClauseResult> execute() override;
};
