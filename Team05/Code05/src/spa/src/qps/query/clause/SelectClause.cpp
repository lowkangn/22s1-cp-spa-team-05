#include "SelectClause.h"
#include "PQLEntity.h"
#include "EntityClauseResult.h"
#include <pkb/PKB.h>

shared_ptr<EntityClauseResult> SelectClause::execute(shared_ptr<PKB> pkb) {
	if (!toSelect.isSynonym()) {
		throw PQLError("SelectClause argument is not a synonym");
	}

	vector<PQLEntity> entities;

	if (toSelect.isStmtSynonym()) {
		entities = pkb->retrieveAllStatementEntities();
	} else if (toSelect.isProcedureSynonym()) {
		entities = pkb->retrieveAllProcedureEntities();
	} else if (toSelect.isVariableSynonym()) {
		entities = pkb->retrieveAllVariables();
	} else if (toSelect.isConstantSynonym()) {
		entities = pkb->retrieveAllConstants();
	} else {
		if (toSelect.isReadSynonym()) {
			entities = pkb->retrieveStatementEntitiesByType(PKBTrackedStatementType::READ);
		} else if (toSelect.isPrintSynonym()) {
			entities = pkb->retrieveStatementEntitiesByType(PKBTrackedStatementType::PRINT);
		} else if (toSelect.isAssignSynonym()) {
			entities = pkb->retrieveStatementEntitiesByType(PKBTrackedStatementType::ASSIGN);
		} else if (toSelect.isCallSynonym()) {
			entities = pkb->retrieveStatementEntitiesByType(PKBTrackedStatementType::CALL);
		} else if (toSelect.isWhileSynonym()) {
			entities = pkb->retrieveStatementEntitiesByType(PKBTrackedStatementType::WHILE);
		} else if (toSelect.isIfSynonym()) {
			entities = pkb->retrieveStatementEntitiesByType(PKBTrackedStatementType::IF);
		} else {
			throw PQLError("Cannot identify SelectClause argument");
		}
	}

    return shared_ptr<EntityClauseResult>(new EntityClauseResult(toSelect, entities));
}

bool SelectClause::equals(const SelectClause* other) {
	if (dynamic_cast<const SelectClause*>(other) == nullptr) {
		return false;
	}
	SelectClause otherSelect = *dynamic_cast<const SelectClause*>(other);
	return this->toSelect == otherSelect.toSelect;
}
