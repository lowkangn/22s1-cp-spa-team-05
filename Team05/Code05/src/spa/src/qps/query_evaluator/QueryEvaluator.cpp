#include <set>
#include "QueryEvaluator.h"

set<string> QueryEvaluator::combine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>> results) {

    EntityClauseResult entitiesResult = dereference(results.first);
    list<RelationshipClauseResult> relationshipsResults = dereference(results.second);

    // If result from SelectClause returns no entries, return empty set
    if (entitiesResult.isEmpty()) {
        return set<string>();
    }

    // If result from any other Clause returns no entries, return empty set
    for (RelationshipClauseResult relationshipsResult : relationshipsResults) {
        if (relationshipsResult.isEmpty()) {
            return set<string>();
        }
    }

    set<PQLEntity> entitiesToReturn;

    list<RelationshipClauseResult>::iterator relationshipsResultIter = relationshipsResults.begin();
    for (; relationshipsResultIter != relationshipsResults.end(); relationshipsResultIter++) {
        // Check if either of its ClauseArguments match the EntityClauseResult's ClauseArgument
        RelationshipArgument argumentFound = findDesiredArgument(entitiesResult.getArg(),
                                                                 *relationshipsResultIter);

        // If neither ClauseArgument matches, skip this RelationshipClauseResult
        if (argumentFound == RelationshipArgument::NONE) {
            continue;
        } else {
            // Extract entities corresponding to the matching ClauseArgument from the RelationshipClauseResult
            set<PQLEntity> entitiesToIntersect = extractEntitySet(argumentFound, relationshipsResultIter->getRelationships());

            // Intersect sets into entitiesToReturn
            if (relationshipsResultIter == relationshipsResults.begin()) {
                entitiesToReturn = entitiesToIntersect;
            } else {
               entitiesToReturn = intersectSets(entitiesToReturn, entitiesToIntersect);

               // If intersection returns empty set, no entities meet all constraints so return empty set
               if (entitiesToReturn.empty()) {
                   return set<string>();
               }
            }
        }
    }

    // If at this stage entitiesToReturn is empty, means that all RelationshipClauseResults were skipped
    // Hence, entitiesToReturn is just the list of entities in entitiesResult
    if (entitiesToReturn.empty()) {
        vector<PQLEntity> selectedEntities = entitiesResult.getEntities();
        for (PQLEntity entity : selectedEntities) {
            entitiesToReturn.insert(entity);
        }
    }

    // Convert set of entities into set of entity strings
    set<string> entityStringsToReturn;

    for (PQLEntity entity : entitiesToReturn) {
        entityStringsToReturn.insert(entity.toString());
    }

    return entityStringsToReturn;
}

set<string> QueryEvaluator::evaluate(Query query) {
    pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>> results = query.execute();
	return combine(results);
}
