#include <qps/query/clause/SelectClause.h>

#include <memory>

list<shared_ptr<ClauseResult>> SelectClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	list<shared_ptr<ClauseResult>> resultList;
	// no results to retrieve for select boolean
	if (isBooleanReturnType) {
		return resultList;
	}

	list<ClauseArgument>::iterator argsIter = this->selectArgs.begin();
	while (argsIter != this->selectArgs.end()) {
		ClauseArgument synonym = *argsIter;
		
		// check arg immediately after synonym
		argsIter++;
		if (argsIter != this->selectArgs.end() && argsIter->isAttributeName()) {
			// if arg after synonym is an attribute, they form an attrRef so we retrieve the attrRef
			resultList.push_back(this->getSingleAttrRefResult(pkb, synonym, *argsIter));
			argsIter++;
		} else {
			// else, retrieve the synonym
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

	if (toSelect.isProcedureSynonym()) {
		entities = pkb->retrieveAllProcedureEntities();
	} else if (toSelect.isVariableSynonym()) {
		entities = pkb->retrieveAllVariables();
	} else if (toSelect.isConstantSynonym()) {
		entities = pkb->retrieveAllConstants();
	} else if (toSelect.isStmtRefNoWildcard()) {
		PKBTrackedStatementType stmtType = this->getPKBStmtType(toSelect);
		entities = pkb->retrieveStatementEntitiesByType(stmtType);
	} else {
			throw PQLLogicError("Cannot identify SelectClause argument");
	}

    return make_shared<EntityClauseResult>(toSelect, entities);
}

bool SelectClause::equals(shared_ptr<SelectClause> other) {
	if (other == nullptr) {
		return false;
	}
	return this->isBooleanReturnType == other->isBooleanReturnType
		&& this->selectArgs == other->selectArgs;
}
