#pragma once

#include <assert.h>

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>
#include <qps/query/clause/PQLEntity.h>

#include <algorithm>
#include <vector>

class EntityClauseResult : public ClauseResult {
private:
    ClauseArgument getArg() {
        assert(this->args.size() == 1);
        return this->args[0];
    }

public:
    /* Creates a non empty result with no synonyms.
     * Equivalent to a clause result that poses no constraint on the final query result */
    static EntityClauseResult createNonEmptyNoSynonymResult() {
        int aConstantThatDoesntMatter = 0;
        PQLEntity whateverEntityYouLike = PQLEntity::generateConstant(aConstantThatDoesntMatter);
        vector<PQLEntity> notEmptyVector{whateverEntityYouLike};
        return EntityClauseResult(ClauseArgument::createWildcardArg(), notEmptyVector);
    }

    /* Creates an empty result with no synonyms.
     * Equivalent to a clause result that makes the final query result empty */
    static EntityClauseResult createEmptyNoSynonymResult() {
        return EntityClauseResult(ClauseArgument::createWildcardArg(), vector<PQLEntity>{});
    }

    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : ClauseResult(
        {arg},
        entities) {
    }

    friend bool operator==(EntityClauseResult first, EntityClauseResult second);

    friend bool operator<(EntityClauseResult first, EntityClauseResult second);
};
