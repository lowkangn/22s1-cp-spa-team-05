#include "ModifiesPClause.h"
#include "PQLRelationship.h"
#include "RelationshipClauseResult.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"

shared_ptr<RelationshipClauseResult> ModifiesPClause::execute() {
	// TODO: Waiting for PKB interaction implementation
    return shared_ptr<RelationshipClauseResult>();
}

bool ModifiesPClause::equals(const SuchThatClause* other) {
	if (dynamic_cast<const ModifiesPClause*>(other) == nullptr) {
		return false;
	}
	ModifiesPClause otherModifiesP = *dynamic_cast<const ModifiesPClause*>(other);
	return this->lhs == otherModifiesP.lhs && this->rhs == otherModifiesP.rhs;
}