#pragma once

#include "Clause.h"

class FollowsTClause : public Clause {
public:
    shared_ptr<ClauseResult> execute() override;
};
