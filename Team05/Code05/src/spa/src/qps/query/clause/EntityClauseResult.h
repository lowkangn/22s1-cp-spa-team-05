#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>
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
    static EntityClauseResult createNonEmptyNoSynonymResult() {
        vector<PQLEntity> nonEmptyDummyEntities{ PQLEntity::generateConstant(0) };
        return EntityClauseResult(ClauseArgument::createWildcardArg(), nonEmptyDummyEntities);
    }

    static EntityClauseResult createEmptyNoSynonymResult() {
        return EntityClauseResult(ClauseArgument::createWildcardArg(), vector<PQLEntity>{});
    }
    
    EntityClauseResult(ClauseArgument arg, vector<PQLEntity> entities) : ClauseResult({arg}, entities) {};

	friend bool operator==(EntityClauseResult first, EntityClauseResult second);

	friend bool operator<(EntityClauseResult first, EntityClauseResult second);
};
