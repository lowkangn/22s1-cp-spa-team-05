#include "SelectClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "PQLEntity.h"
#include "EntityClauseResult.h"

shared_ptr<EntityClauseResult> SelectClause::execute() {
    // TODO: Waiting for PKB interaction implementation
    return shared_ptr<EntityClauseResult>();
}

bool SelectClause::equals(const SelectClause* other) {
	if (dynamic_cast<const SelectClause*>(other) == nullptr) {
		return false;
	}
	SelectClause otherSelect = *dynamic_cast<const SelectClause*>(other);
	return this->toSelect == otherSelect.toSelect;
}
