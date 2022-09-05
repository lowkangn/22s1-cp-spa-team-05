#pragma once

#include <vector>
#include "PQLEntity.h"
#include "ClauseResult.h"
#include "ClauseArgument.h"

class EntityClauseResult : public ClauseResult {

private:
    ClauseArgument arg;
    vector<PQLEntity> entities;

public:
    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : arg(arg), entities(entities) {
        this->arg = arg;
        this->entities = entities;
    }

    ClauseArgument getArg() {
        return arg;
    }
    vector<PQLEntity> getEntities() {
        return entities;
    }

};

