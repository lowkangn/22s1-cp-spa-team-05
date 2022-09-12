#include "ModifiesSClause.h"
#include "PQLRelationship.h"
#include "RelationshipClauseResult.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"

shared_ptr<RelationshipClauseResult> ModifiesSClause::execute() {
    // Not sure of the correct way to call this
//    vector<PQLRelationship> relationships = PKBQueryHandler().retrieveKnowledge(ClauseType::ModifiesS, lhs, rhs);
//    return shared_ptr<ClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
    return shared_ptr<RelationshipClauseResult>();
}

bool ModifiesSClause::equals (const SuchThatClause* other) {
	if (dynamic_cast<const ModifiesSClause*>(other) == nullptr) {
		return false;
	}
	ModifiesSClause otherModifies = *dynamic_cast<const ModifiesSClause*>(other);
	return this->lhs == otherModifies.lhs && this->rhs == otherModifies.rhs;
}
