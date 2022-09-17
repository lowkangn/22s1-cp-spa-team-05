#include <qps/query/clause/RelationshipClauseResult.h>

bool operator==(RelationshipClauseResult first, RelationshipClauseResult second) {
    
    vector<PQLRelationship> firstRelations = first.relationships;
    vector<PQLRelationship> secondRelations = second.relationships;

    // sort the vectors so they can be element-wise compared
    sort(firstRelations.begin(), firstRelations.end());
    sort(secondRelations.begin(), secondRelations.end());

    return first.firstArg == second.firstArg && first.secondArg == second.secondArg && firstRelations == secondRelations;
}
