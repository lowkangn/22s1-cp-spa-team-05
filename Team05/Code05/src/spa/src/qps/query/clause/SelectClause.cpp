#include "SelectClause.h"
#include "../../../pkb/interfaces/PKBQueryHandler.h"
#include "PQLEntity.h"
#include "EntityClauseResult.h"

shared_ptr<EntityClauseResult> SelectClause::execute() {
    // Not sure of the correct way to call this
//    vector<PQLEntity> entities = PKBQueryHandler().retrieveKnowledge(toSelect);
//    return shared_ptr<ClauseResult>(new EntityClauseResult(toSelect, entities));
    return shared_ptr<EntityClauseResult>();
}

bool SelectClause::equals(const SelectClause* other) {
	if (dynamic_cast<const SelectClause*>(other) == nullptr) {
		return false;
	}
	SelectClause otherSelect = *dynamic_cast<const SelectClause*>(other);
	return this->toSelect == otherSelect.toSelect;
}
