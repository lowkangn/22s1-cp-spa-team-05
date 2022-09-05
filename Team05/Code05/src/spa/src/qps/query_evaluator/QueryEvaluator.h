#pragma once

#include <string>
#include <vector>

#include "../query/clause/ClauseResult.h"
#include "../query/clause/EntityClauseResult.h"
#include "../query/clause/RelationshipClauseResult.h"
#include "../query/Query.h"

using namespace std;

enum class DeclarationCheckOutput {
    ARG1,
    ARG2,
    NONE
};

class QueryEvaluator {
private:
    DeclarationCheckOutput checkDeclaration(shared_ptr<EntityClauseResult> entityResult,
                                            shared_ptr<RelationshipClauseResult> relationshipResult) {
        if (entityResult->getArg() == relationshipResult->getFirstArg()) {
            return DeclarationCheckOutput::ARG1;
        } else if (entityResult->getArg() == relationshipResult->getSecondArg()) {
            return DeclarationCheckOutput::ARG2;
        } else {
            return DeclarationCheckOutput::NONE;
        }
    }

    void filterEntitiesToReturn(unordered_set<string>* currEntitiesToReturn,
                                shared_ptr<EntityClauseResult> entitiesResult,
                                shared_ptr<RelationshipClauseResult> relationshipsResult);
public:
	/* Returns the final result of a query */
	string evaluate(Query query);

	/* Combines the results from the clauses of a query */
	string combine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>);

};
