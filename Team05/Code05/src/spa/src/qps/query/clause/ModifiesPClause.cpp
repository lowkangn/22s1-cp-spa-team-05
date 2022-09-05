#include "ModifiesPClause.h"
#include "PQLRelationship.h"
#include "RelationshipClauseResult.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"

shared_ptr<ClauseResult> ModifiesPClause::execute() {
    // Not sure of the correct way to call this
//    vector<PQLRelationship> relationships = PKBQueryHandler().retrieveKnowledge(ClauseType::ModifiesP, lhs, rhs);
//    return shared_ptr<ClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
    return shared_ptr<ClauseResult>();
}

bool ModifiesPClause::equals(const Clause* other) {
	if (dynamic_cast<const ModifiesPClause*>(other) == nullptr) {
		return false;
	}
	ModifiesPClause otherModifiesP = *dynamic_cast<const ModifiesPClause*>(other);
	return this->lhs == otherModifiesP.lhs && this->rhs == otherModifiesP.rhs;
}