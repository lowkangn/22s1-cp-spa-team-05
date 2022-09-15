#include <qps/query_evaluator/QueryEvaluator.h>

set<string> QueryEvaluator::evaluate(Query query, shared_ptr<PKBQueryHandler> pkb) {
	shared_ptr<EntityClauseResult> entitiesResultPointer = query.executeSelect(pkb);
	list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers = query.executeSuchThat(pkb);
	return combine(entitiesResultPointer, relationshipsResultPointers);
}

set<string> QueryEvaluator::combine(shared_ptr<EntityClauseResult> entitiesResultPointer,
									list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers) {

    EntityClauseResult entitiesResult = *entitiesResultPointer;
    list<RelationshipClauseResult> relationshipsResults =
			this->dereferenceRelationshipsResultPointers(relationshipsResultPointers);

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
        RelationshipArgument argumentFound = this->findDesiredArgument(entitiesResult.getArg(),
                                                                 *relationshipsResultIter);

        // If neither ClauseArgument matches, skip this RelationshipClauseResult
        if (argumentFound == RelationshipArgument::NONE) {
            continue;
        } else {
            // Extract entities corresponding to the matching ClauseArgument from the RelationshipClauseResult
            set<PQLEntity> entitiesToIntersect = this->extractEntitySet(
					argumentFound, relationshipsResultIter->getRelationships());

            // Intersect sets into entitiesToReturn
            if (relationshipsResultIter == relationshipsResults.begin()) {
                entitiesToReturn = entitiesToIntersect;
            } else {
               entitiesToReturn = this->intersectSets(entitiesToReturn, entitiesToIntersect);

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

list<RelationshipClauseResult> QueryEvaluator::dereferenceRelationshipsResultPointers(
		list<shared_ptr<RelationshipClauseResult>> relationshipsResultPointers) {
	list<RelationshipClauseResult> relationshipsToReturn;
	for (shared_ptr<RelationshipClauseResult> relationshipsResultPointer : relationshipsResultPointers) {
		relationshipsToReturn.push_back(*relationshipsResultPointer);
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
