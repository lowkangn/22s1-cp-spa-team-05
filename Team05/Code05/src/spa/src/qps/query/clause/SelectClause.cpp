#include "SelectClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "../../qps/query/clause/PQLEntity.h"

ClauseResult SelectClause::execute() {
    // Not sure of the correct way to call this
    vector<PQLEntity> entries = PKBQueryHandler::retrieveKnowledge(arg);
    // ...
    return *new ClauseResult();
}
