#include "ModifiesSClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "PQLRelationship.h"
#include "RelationshipClauseResult.h"

shared_ptr<ClauseResult> ModifiesSClause::execute() {
    // Not sure of the correct way to call this
    vector<PQLRelationship> relationships = PKBQueryHandler::retrieveKnowledge(ClauseType::ModifiesS, lhs, rhs);
    return shared_ptr<ClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool ModifiesSClause::equals (const Clause* other) {
	if (dynamic_cast<const ModifiesSClause*>(other) == nullptr) {
		return false;
	}
	ModifiesSClause otherModifies = *dynamic_cast<const ModifiesSClause*>(other);
	return this->lhs == otherModifies.lhs && this->rhs == otherModifies.rhs;
};