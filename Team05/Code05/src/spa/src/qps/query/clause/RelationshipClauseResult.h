#pragma once

#include <assert.h>

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>
#include <qps/query/clause/PQLRelationship.h>

#include <algorithm>
#include <vector>

class RelationshipClauseResult : public ClauseResult {
private:
    ClauseArgument getFirstArg() {
        assert(this->args.size() == 2);
        return this->args[0];
    }

    ClauseArgument getSecondArg() {
        assert(this->args.size() == 2);
        return this->args[1];
    }

public:
    RelationshipClauseResult(
        ClauseArgument firstArg, ClauseArgument secondArg,
        vector<PQLRelationship> relationships) : ClauseResult(
        {firstArg, secondArg},
        relationships) {
    }

    friend bool operator==(RelationshipClauseResult first, RelationshipClauseResult second);
};
