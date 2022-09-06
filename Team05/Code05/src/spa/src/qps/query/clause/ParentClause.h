#pragma once

#include "Clause.h"

class ParentClause : public Clause {
public:
    shared_ptr<ClauseResult> execute() override;
};
