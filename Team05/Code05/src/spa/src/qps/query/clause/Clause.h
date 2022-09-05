#pragma once
#include <qps/query/clause/ClauseResult.h>

class Clause {
public:
	/* Returns the items retrieved from the PKB as a ClauseResult. */
	virtual ClauseResult execute() = 0;

	virtual bool equals(const Clause* other) = 0;
};