#include "SelectClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "../../qps/query/clause/PQLEntity.h"
#include "EntityClauseResult.h"

shared_ptr<ClauseResult> SelectClause::execute() {
    // Not sure of the correct way to call this
    vector<PQLEntity> entities = PKBQueryHandler::retrieveKnowledge(arg);
    return shared_ptr<ClauseResult>(new EntityClauseResult(arg, entities));
}
