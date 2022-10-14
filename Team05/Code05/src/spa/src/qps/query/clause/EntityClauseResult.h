#pragma once

#include <vector>
#include <algorithm>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

class EntityClauseResult : public ClauseResult {

private:
    ClauseArgument arg;
    vector<PQLEntity> entities;

public:

    static EntityClauseResult createNonEmptyNoSynonymResult() {
        vector<PQLEntity> nonEmptyDummyEntities{ PQLEntity::generateConstant(0) };
        return EntityClauseResult(ClauseArgument::createWildcardArg(), nonEmptyDummyEntities);
    }

    static EntityClauseResult createEmptyNoSynonymResult() {
        return EntityClauseResult(ClauseArgument::createWildcardArg(), vector<PQLEntity>{});
    }
    
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

	friend bool operator<(EntityClauseResult first, EntityClauseResult second);
};
