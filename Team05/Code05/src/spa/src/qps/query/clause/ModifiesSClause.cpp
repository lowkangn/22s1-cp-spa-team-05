#include "ModifiesSClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "../../qps/query/clause/PQLRelationship.h"
#include "RelationshipClauseResult.h"

shared_ptr<ClauseResult> ModifiesSClause::execute() {
    // Not sure of the correct way to call this
    vector<PQLRelationship> relationships = PKBQueryHandler::retrieveKnowledge(ClauseType::ModifiesS, lhs, rhs);
    return shared_ptr<ClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));

}