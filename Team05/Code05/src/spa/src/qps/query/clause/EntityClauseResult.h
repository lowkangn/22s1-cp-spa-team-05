#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

class EntityClauseResult : public ClauseResult {
private:
	ClauseArgument getArg() {
		assert(this->args.size() == 1);
		return this->args[0];
	}

public:
    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : ClauseResult({arg}, entities) {};

	friend bool operator==(EntityClauseResult first, EntityClauseResult second);

	friend bool operator<(EntityClauseResult first, EntityClauseResult second);
};
