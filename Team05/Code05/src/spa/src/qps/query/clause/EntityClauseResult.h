#pragma once

#include <vector>
#include <algorithm>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

class EntityClauseResult : public ClauseResult {
private:
	ClauseArgument getArg() {
		return this->args[0];
	}

public:
    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : ClauseResult({arg}, entities) {};

	friend bool operator==(EntityClauseResult first, EntityClauseResult second);

	friend bool operator<(EntityClauseResult first, EntityClauseResult second);
};
