#include <unordered_set>
#include "QueryEvaluator.h"

unordered_set<string> QueryEvaluator::combine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>> results) {

    // Safe cast as we know results.first is the result of SelectClause's execute() which returns a ClauseResult pointer
    // pointing to an EntityClauseResult: https://stackoverflow.com/questions/1358143/downcasting-shared-ptrbase-to-shared-ptrderived
    // A bit sus but will work for now until we can do restructuring of Clause + ClauseResult
    shared_ptr<EntityClauseResult> entitiesResult = static_pointer_cast<EntityClauseResult>(results.first);

    list<shared_ptr<RelationshipClauseResult>> relationshipsResults;
    while (!results.second.empty()) {
        // Safe cast as we know results.second is the result of execute() (excluding SelectClause) which returns a
        // ClauseResult pointer pointing to a RelationshipClauseResult.
        shared_ptr<RelationshipClauseResult> relationshipsResult = static_pointer_cast<RelationshipClauseResult>(
                results.second.front());
        relationshipsResults.push_back(relationshipsResult);
        results.second.pop_front();
    }

    unordered_set<string> entityNamesToReturn;

    // If result from SelectClause returns no entries, return "None"
    if (entitiesResult->getEntities().empty()) return entityNamesToReturn;

    // If result from any other Clause returns no entries, return "None"
    list<shared_ptr<RelationshipClauseResult>>::iterator checkEmptyIter = relationshipsResults.begin();
    for (; checkEmptyIter != relationshipsResults.end(); checkEmptyIter++) {
        if ((*checkEmptyIter)->getRelationships().empty()) return entityNamesToReturn;
    }

    // We start with a list of entity names from the SelectClause, then remove as we check the other clauses for constraints
    vector<PQLEntity> entities = entitiesResult->getEntities();

    vector<PQLEntity>::iterator createReturnVectorIter = entities.begin();
    for (; createReturnVectorIter != entities.end(); createReturnVectorIter++) {
        entityNamesToReturn.insert(createReturnVectorIter->toString());
    }

    // Iterate through the other clauses
    list<shared_ptr<RelationshipClauseResult>>::iterator filterIter = relationshipsResults.begin();
    for (; filterIter != relationshipsResults.end(); filterIter++) {
        filterEntitiesToReturn(&entityNamesToReturn, entitiesResult, *filterIter);
    }

    return entityNamesToReturn;
}

unordered_set<string> QueryEvaluator::evaluate(Query query) {
    pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>> results = query.execute();
	return combine(results);
}

void QueryEvaluator::filterEntitiesToReturn(unordered_set<string>* currEntitiesToReturn, shared_ptr<EntityClauseResult> entitiesResult,
                            shared_ptr<RelationshipClauseResult> relationshipsResult) {

    DeclarationCheckOutput output = checkDeclaration(entitiesResult, relationshipsResult);

    // If clause does not share a variable with the SelectClause, skip it
    if (output == DeclarationCheckOutput::NONE) {
        return;

    } else {
        // Obtain an unordered set of names of the relevant entities in this relationships vector
        vector<PQLRelationship> relationships = relationshipsResult->getRelationships();
        unordered_set<string> entity_set;

        vector<PQLRelationship>::iterator createSetIter = relationships.begin();

        if (output == DeclarationCheckOutput::ARG1) {
            for (; createSetIter != relationships.end(); createSetIter++) {
                PQLEntity currEntity = createSetIter->getFirstEntity();
                entity_set.insert(currEntity.toString());
            }
        } else {
            for (; createSetIter != relationships.end(); createSetIter++) {
                PQLEntity currEntity = createSetIter->getSecondEntity();
                entity_set.insert(currEntity.toString());
            }
        }

        // If a name in entityNamesToReturn is not in the set of names of the entities returned by this clause, remove it
        unordered_set<string>::iterator removeIter = currEntitiesToReturn->begin();
        for (; removeIter != currEntitiesToReturn->end(); removeIter++) {
            if (entity_set.find(*removeIter) == entity_set.end()) {
                currEntitiesToReturn->erase(*removeIter);
            }
        }
    }
}
