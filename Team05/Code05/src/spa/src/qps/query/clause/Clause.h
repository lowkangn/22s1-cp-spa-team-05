#pragma once
#include "ClauseResult.h"

using namespace std;

class Clause {
public:
	/* Returns the items retrieved from the PKB as a ClauseResult. */
	virtual shared_ptr<ClauseResult> execute() = 0;

	virtual bool equals(const Clause* other) = 0;
};