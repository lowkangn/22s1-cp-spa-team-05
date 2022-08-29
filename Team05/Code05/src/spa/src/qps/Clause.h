#pragma once
#include <ClauseResult.h>

class Clause {
public:
	/* Returns the items retrieved from the PKB as a ClauseResult. */
	virtual ClauseResult execute();
};