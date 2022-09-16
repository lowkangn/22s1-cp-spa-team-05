#include <qps/query/clause/RelationshipClauseResult.h>

bool operator==(RelationshipClauseResult first, RelationshipClauseResult second) {
    vector<PQLRelationship> firstRelations = first.relationships;
    sort(firstRelations.begin(), firstRelations.end());
    vector<PQLRelationship> secondRelations = second.relationships;
    sort(secondRelations.begin(), secondRelations.end());
    return first.firstArg == second.firstArg && first.secondArg == second.secondArg && firstRelations == secondRelations;
}
