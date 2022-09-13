#include "ModifiesSClause.h"
#include "PQLRelationship.h"
#include "RelationshipClauseResult.h"

shared_ptr<RelationshipClauseResult> ModifiesSClause::execute() {
	// TODO: Waiting for PKB interaction implementation
    return shared_ptr<RelationshipClauseResult>();
}

bool ModifiesSClause::equals(const SuchThatClause* other) {
	if (dynamic_cast<const ModifiesSClause*>(other) == nullptr) {
		return false;
	}
	ModifiesSClause otherModifies = *dynamic_cast<const ModifiesSClause*>(other);
	return this->lhs == otherModifies.lhs && this->rhs == otherModifies.rhs;
}
