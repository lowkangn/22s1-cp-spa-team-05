#pragma once

#include <vector>
#include "PQLRelationship.h"
#include "ClauseArgument.h"

class RelationshipClauseResult {

private:
    ClauseArgument firstArg;
    ClauseArgument secondArg;
    vector<PQLRelationship> relationships;

public:
    RelationshipClauseResult(ClauseArgument firstArg, ClauseArgument secondArg, vector<PQLRelationship> relationships)
                                : firstArg(firstArg), secondArg(secondArg), relationships(relationships) {
        this->firstArg = firstArg;
        this->secondArg = secondArg;
        this->relationships = relationships;
    }

    ClauseArgument getFirstArg() {
        return this->firstArg;
    }

    ClauseArgument getSecondArg() {
        return this->secondArg;
    }

    vector<PQLRelationship> getRelationships() {
        return this->relationships;
    }

    friend bool operator==(RelationshipClauseResult first, RelationshipClauseResult second);
};
