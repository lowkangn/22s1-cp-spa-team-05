#pragma once

#include <string>
#include <vector>
#include <set>

#include "../query/clause/ClauseResult.h"
#include "../query/clause/EntityClauseResult.h"
#include "../query/clause/RelationshipClauseResult.h"
#include "../query/Query.h"

using namespace std;

enum class RelationshipArgument {
    ARG1,
    ARG2,
    NONE
};

class QueryEvaluator {
private:
    RelationshipArgument findDesiredArgument(ClauseArgument desiredArg,
                                            RelationshipClauseResult relationshipResultToCheck) {
        if (desiredArg == relationshipResultToCheck.getFirstArg()) {
            return RelationshipArgument::ARG1;
        } else if (desiredArg == relationshipResultToCheck.getSecondArg()) {
            return RelationshipArgument::ARG2;
        } else {
            return RelationshipArgument::NONE;
        }
    }

    EntityClauseResult dereference(shared_ptr<ClauseResult> entitiesResultPointer) {
        // Safe cast as we know entitiesResultPointer is the result of SelectClause's execute() which returns a
        // ClauseResult pointer pointing to an EntityClauseResult.
        // https://stackoverflow.com/questions/1358143/downcasting-shared-ptrbase-to-shared-ptrderived
        return *(static_pointer_cast<EntityClauseResult>(entitiesResultPointer));
    }

    list<RelationshipClauseResult> dereference(list<shared_ptr<ClauseResult>> relationshipsResultPointers) {
        list<RelationshipClauseResult> relationshipsToReturn;
        for (shared_ptr<ClauseResult> relationshipsResultPointer : relationshipsResultPointers) {
            // Safe cast as we know relationshipsResultPointers are the result of execute() (excluding SelectClause)
            // which returns a ClauseResult pointer pointing to a RelationshipClauseResult.
            relationshipsToReturn.push_back(*(static_pointer_cast<RelationshipClauseResult>(relationshipsResultPointer)));
        }
        return relationshipsToReturn;
    }

    set<PQLEntity> extractEntitySet(RelationshipArgument arg, vector<PQLRelationship> relationships) {
        set<PQLEntity> setToReturn;
        if (arg == RelationshipArgument::ARG1) {
            for (PQLRelationship relationship : relationships) {
                setToReturn.insert(relationship.getFirstEntity());
            }
        } else {
            for (PQLRelationship relationship : relationships) {
                setToReturn.insert(relationship.getSecondEntity());
            }
        }
        return setToReturn;
    }

    set<PQLEntity> intersectSets(set<PQLEntity> currentSet, set<PQLEntity> setToIntersect) {
        set<PQLEntity> temp;
        set_intersection(currentSet.begin(), currentSet.end(),
                         setToIntersect.begin(), setToIntersect.end(),
                         inserter(temp, temp.begin()));
        return temp;
    }
public:
    QueryEvaluator() {};

	/* Returns the final result of a query */
	set<string> evaluate(Query query);

	/* Combines the results from the clauses of a query */
    set<string> combine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>);

};
