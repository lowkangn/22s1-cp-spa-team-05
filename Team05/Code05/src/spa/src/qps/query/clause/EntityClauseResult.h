#pragma once

#include <vector>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

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
        return this->arg;
    }

    vector<PQLEntity> getEntities() {
        return this->entities;
    }

    bool hasArg(ClauseArgument argToFind) {
        return this->arg == argToFind;
    }

    bool isEmpty() {
        return this->entities.empty();
    }

    friend bool operator==(EntityClauseResult first, EntityClauseResult second);
};
