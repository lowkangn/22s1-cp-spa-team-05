#pragma once

#include "Clause.h"

class UsesPClause : public Clause {
public:
	shared_ptr<ClauseResult> execute() override;
};
