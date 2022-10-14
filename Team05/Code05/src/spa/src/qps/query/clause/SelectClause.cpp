#include <qps/query/clause/SelectClause.h>

#include <memory>

list<shared_ptr<ClauseResult>> SelectClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<ClauseResult>> resultList;
	if (isBooleanReturnType) {
		return resultList;
	}
	list<ClauseArgument>::iterator argsIter = this->selectArgs.begin();
	while (argsIter != this->selectArgs.end()) {
		ClauseArgument synonym = *argsIter;
		argsIter++;
		if (argsIter != this->selectArgs.end() && argsIter->isAttributeName()) {
			resultList.push_back(this->getSingleAttrRefResult(pkb, synonym, *argsIter));
			argsIter++;
		} else {
			resultList.push_back(this->getSingleEntityResult(pkb, synonym));
		}
	}
	return resultList;
}

shared_ptr<EntityClauseResult> SelectClause::getSingleEntityResult(shared_ptr<PKBQueryHandler> pkb, ClauseArgument toSelect) {
	if (!toSelect.isSynonym()) {
		throw PQLSyntaxError("SelectClause argument is not a synonym");
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
			throw PQLLogicError("Cannot identify SelectClause argument");
		}
	}

    return make_shared<EntityClauseResult>(toSelect, entities);
}

bool SelectClause::equals(shared_ptr<SelectClause> other) {
	if (dynamic_pointer_cast<SelectClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<SelectClause> otherSelect = dynamic_pointer_cast<SelectClause>(other);

	if (this->isBooleanReturnType != otherSelect->isBooleanReturnType) {
		return false;
	}

	list<ClauseArgument>::iterator thisIter = this->selectArgs.begin();
	list<ClauseArgument>::iterator otherIter = otherSelect->selectArgs.begin();

	bool isArgEqual;
	while (thisIter != this->selectArgs.end()) {
		isArgEqual = (*thisIter == *otherIter);
		if (!isArgEqual) {
			return false;
		}
		thisIter++;
		otherIter++;
	}
	return true;
}
