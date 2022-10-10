#pragma once

#include <vector>
#include <algorithm>
#include <qps/query/clause/PQLRelationship.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseResult.h>

class RelationshipClauseResult : public ClauseResult {

private:
    ClauseArgument firstArg;
    ClauseArgument secondArg;
    vector<PQLRelationship> relationships;
    bool isRelationshipsEmpty;

    RelationshipClauseResult(ClauseArgument firstArg, ClauseArgument secondArg) 
        : firstArg(firstArg), secondArg(secondArg) {
        this->isRelationshipsEmpty = false;
    }

public:
    RelationshipClauseResult(ClauseArgument firstArg, ClauseArgument secondArg, vector<PQLRelationship> relationships)
                                : firstArg(firstArg), secondArg(secondArg), relationships(relationships) {
        this->firstArg = firstArg;
        this->secondArg = secondArg;
        this->relationships = relationships;
        this->isRelationshipsEmpty = relationships.empty();
    }

    RelationshipClauseResult generateNonEmptyDummyResult(ClauseArgument firstArg, ClauseArgument secondArg) {
        return RelationshipClauseResult(firstArg, secondArg);
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

    bool isEmpty() {
        return this->isRelationshipsEmpty;
    }

    friend bool operator==(RelationshipClauseResult first, RelationshipClauseResult second);
};
