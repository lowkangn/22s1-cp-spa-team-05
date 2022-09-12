#pragma once

#include <vector>
#include "PQLEntity.h"
#include "ClauseArgument.h"

class EntityClauseResult {

private:
    ClauseArgument arg;
    vector<PQLEntity> entities;

public:
    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : arg(arg), entities(entities) {
        this->arg = arg;
        this->entities = entities;
    }

    ClauseArgument getArg() {
        return this->arg;
    }
    vector<PQLEntity> getEntities() {
        return this->entities;
    }

    friend bool operator==(EntityClauseResult first, EntityClauseResult second);
};
