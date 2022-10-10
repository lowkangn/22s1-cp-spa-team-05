#pragma once

#include <vector>
#include <algorithm>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

class EntityClauseResult : public ClauseResult {
public:
    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : ClauseResult({arg}, entities) {};

    ClauseArgument getArg() {
        return this->args[0];
    }

    friend bool operator==(EntityClauseResult first, EntityClauseResult second);

	friend bool operator<(EntityClauseResult first, EntityClauseResult second);
};
