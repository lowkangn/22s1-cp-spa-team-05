#pragma once

#include "Clause.h"

class UsesSClause : public Clause {
public:
	shared_ptr<ClauseResult> execute() override;
};
