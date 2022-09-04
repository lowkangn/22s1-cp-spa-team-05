#include "ModifiesSClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "../../qps/query/clause/PQLRelationship.h"

ClauseResult ModifiesSClause::execute() {
    // Not sure of the correct way to call this
    vector<PQLRelationship> entries = PKBQueryHandler::retrieveKnowledge(clauseType, lhs, rhs);
    // ...
    return *new ClauseResult();
}
