#pragma once

#include <vector>
#include <algorithm>
#include <qps/query/clause/PQLRelationship.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

class RelationshipClauseResult : public ClauseResult {
public:
    RelationshipClauseResult(ClauseArgument firstArg, ClauseArgument secondArg, vector<PQLRelationship> relationships)
                                : ClauseResult({firstArg, secondArg}, relationships) {};

    ClauseArgument getFirstArg() {
        return this->args[0];
    }

    ClauseArgument getSecondArg() {
        return this->args[1];
    }

    friend bool operator==(RelationshipClauseResult first, RelationshipClauseResult second);
};
