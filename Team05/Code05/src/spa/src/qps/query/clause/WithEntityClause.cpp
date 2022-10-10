#include <qps/query/clause/WithEntityClause.h>

shared_ptr<ClauseResult> WithEntityClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	ClauseArgument synonym = this->nonExactArgs.front();
	ClauseArgument attribute = this->nonExactArgs.back();
	assert(synonym.isSynonym() && attribute.isAttributeName && exactArg.isExactReference());

	vector<PQLEntity> entity;
	if (synonym.isProcedureSynonym()) {
		entity.emplace_back(pkb->retrieveProcedureEntityByName(exactArg.getIdentifier()));
	} 
	else if (synonym.isVariableSynonym()) {
		entity.emplace_back(pkb->retrieveVariableByName(exactArg.getIdentifier()));
	}
	else if (synonym.isConstantSynonym()) {
		entity.emplace_back(pkb->retrieveConstantByValue(exactArg.getLineNumber()));
	}
	else if (synonym.isStmtRefNoWildcard()) {
		PKBTrackedStatementType stmtType = this->getPKBStmtType(synonym);
		entity.emplace_back(pkb->retrieveStatementEntityByLineNumber(exactArg.getLineNumber(), stmtType));
	}
	else {
		throw PQLLogicError("Unable to identify synonym type in with clause");
	}

	return make_shared<EntityClauseResult>(synonym, entity);
}

bool WithEntityClause::equals(shared_ptr<WithClause> other) {
	if (dynamic_pointer_cast<WithEntityClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<WithEntityClause> otherWithEntity = dynamic_pointer_cast<WithEntityClause>(other);
	return (this->exactArg == otherWithEntity->exactArg) && (this->nonExactArgs == otherWithEntity->nonExactArgs);
}

PKBTrackedStatementType WithEntityClause::getPKBStmtType(ClauseArgument& stmtSynonymArg) {
	if (stmtSynonymArg.isReadSynonym()) {
		return PKBTrackedStatementType::READ;
	}
	else if (stmtSynonymArg.isPrintSynonym()) {
		return PKBTrackedStatementType::PRINT;
	}
	else if (stmtSynonymArg.isAssignSynonym()) {
		return PKBTrackedStatementType::ASSIGN;
	}
	else if (stmtSynonymArg.isCallSynonym()) {
		return PKBTrackedStatementType::CALL;
	}
	else if (stmtSynonymArg.isWhileSynonym()) {
		return PKBTrackedStatementType::WHILE;
	}
	else if (stmtSynonymArg.isIfSynonym()) {
		return PKBTrackedStatementType::IF;
	}
	else if (stmtSynonymArg.isStmtSynonym()) {
		return PKBTrackedStatementType::ALL;
	}
	else {
		throw PQLLogicError("Cannot identify stmt type in with clause synonym");
	}
}