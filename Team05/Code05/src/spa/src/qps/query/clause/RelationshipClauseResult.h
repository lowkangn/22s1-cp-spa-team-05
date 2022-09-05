#pragma once

#include <vector>
#include "PQLRelationship.h"
#include "ClauseResult.h"
#include "ClauseArgument.h"

class RelationshipClauseResult : public ClauseResult {

private:
    ClauseArgument firstArg;
    ClauseArgument secondArg;
    vector<PQLRelationship> relationships;

public:
    RelationshipClauseResult(ClauseArgument firstArg, ClauseArgument secondArg, vector<PQLRelationship> relationships) {
        this->firstArg = firstArg;
        this->secondArg = secondArg;
        this->relationships = relationships;
    }

    ClauseArgument getFirstArg() {
        return firstArg;
    }

    ClauseArgument getSecondArg() {
        return secondArg;
    }

    vector<PQLRelationship> getRelationships() {
        return relationships;
    }

};

