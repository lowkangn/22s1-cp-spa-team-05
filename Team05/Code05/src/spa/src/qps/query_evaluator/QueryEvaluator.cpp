#include <set>
#include "QueryEvaluator.h"

set<string> QueryEvaluator::evaluate(Query query) {
	shared_ptr<ClauseResult> entitiesResultPointer = query.executeSelect();
	list<shared_ptr<ClauseResult>> relationshipsResultPointers = query.executeSuchThat();
	return combine(entitiesResultPointer, relationshipsResultPointers);
}

set<string> QueryEvaluator::combine(shared_ptr<ClauseResult> entitiesResultPointer,
									list<shared_ptr<ClauseResult>> relationshipsResultPointers) {

    EntityClauseResult entitiesResult = dereferenceEntitiesResultPointer(entitiesResultPointer);
    list<RelationshipClauseResult> relationshipsResults =
			dereferenceRelationshipsResultPointers(relationshipsResultPointers);

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
            set<PQLEntity> entitiesToIntersect = extractEntitySet(
					argumentFound, relationshipsResultIter->getRelationships());

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

RelationshipArgument QueryEvaluator::findDesiredArgument(ClauseArgument desiredArg,
										 RelationshipClauseResult relationshipResultToCheck) {
	if (desiredArg == relationshipResultToCheck.getFirstArg()) {
		return RelationshipArgument::ARG1;
	} else if (desiredArg == relationshipResultToCheck.getSecondArg()) {
		return RelationshipArgument::ARG2;
	} else {
		return RelationshipArgument::NONE;
	}
}

EntityClauseResult QueryEvaluator::dereferenceEntitiesResultPointer(shared_ptr<ClauseResult> entitiesResultPointer) {
	// Safe cast as we know entitiesResultPointer is the result of SelectClause's execute() which returns a
	// ClauseResult pointer pointing to an EntityClauseResult.
	// https://stackoverflow.com/questions/1358143/downcasting-shared-ptrbase-to-shared-ptrderived
	return *(static_pointer_cast<EntityClauseResult>(entitiesResultPointer));
}

list<RelationshipClauseResult> QueryEvaluator::dereferenceRelationshipsResultPointers(
		list<shared_ptr<ClauseResult>> relationshipsResultPointers) {
	list<RelationshipClauseResult> relationshipsToReturn;
	for (shared_ptr<ClauseResult> relationshipsResultPointer : relationshipsResultPointers) {
		// Safe cast as we know relationshipsResultPointers are the result of execute() (excluding SelectClause)
		// which returns a ClauseResult pointer pointing to a RelationshipClauseResult.
		relationshipsToReturn.push_back(
				*(static_pointer_cast<RelationshipClauseResult>(relationshipsResultPointer)));
	}
	return relationshipsToReturn;
}

set<PQLEntity> QueryEvaluator::extractEntitySet(RelationshipArgument argToExtract,
												vector<PQLRelationship> relationships) {
	if (argToExtract == RelationshipArgument::NONE) {
		throw PQLError("Attempted to extract PQLEntity set despite no matching declaration");
	}

	set<PQLEntity> setToReturn;
	if (argToExtract == RelationshipArgument::ARG1) {
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

set<PQLEntity> QueryEvaluator::intersectSets(set<PQLEntity> firstSet, set<PQLEntity> secondSet) {
	set<PQLEntity> combined;
	set_intersection(firstSet.begin(), firstSet.end(),
					 secondSet.begin(), secondSet.end(),
					 inserter(combined, combined.begin()));
	return combined;
}
