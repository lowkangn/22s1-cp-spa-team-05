#include <qps/query/clause/WithClause.h>

RelationshipClauseResult WithClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	if (this->lhsArgs.size() == 1 && this->rhsArgs.size() == 1) {
		return this->executeBothExact(pkb);
	}
	else if (this->lhsArgs.size() == 1 || this->rhsArgs.size() == 1) {
		return this->executeOnlyOneExact(pkb);
	}
	return this->executeNoExact(pkb);
}

RelationshipClauseResult WithClause::executeBothExact(shared_ptr<PKBQueryHandler> pkb) {
	assert(this->lhsArgs.size() == 1 && this->lhsArgs.front().isExactReference());
	assert(this->rhsArgs.size() == 1 && this->rhsArgs.front().isExactReference());

	ClauseArgument lhs = this->lhsArgs.front();
	ClauseArgument rhs = this->rhsArgs.front();
	assert((lhs.isLineNumber && rhs.isLineNumber()) || (lhs.isStringLiteral() && rhs.isStringLiteral()));

	if (lhs != rhs) {
		return RelationshipClauseResult(lhs, rhs, vector<PQLRelationship>{});
	}

	shared_ptr<PQLEntity> entity;
	if (lhs.isLineNumber()) {
		entity = make_shared<PQLEntity>(PQLEntity::generateStatement(lhs.getLineNumber()));
	} else {
		entity = make_shared<PQLEntity>(PQLEntity::generateVariable(lhs.getIdentifier()));
	}
	PQLRelationship relationship = PQLRelationship(*entity, *entity);
	return RelationshipClauseResult(lhs, rhs, vector<PQLRelationship>{relationship});
}

RelationshipClauseResult WithClause::executeOnlyOneExact(shared_ptr<PKBQueryHandler> pkb) {
	assert(this->lhsArgs.size() == 1 || this->rhsArgs.size() == 1);
	assert(this->lhsArgs.size() == 2 || this->rhsArgs.size() == 2);

	ClauseArgument exactArg = this->lhsArgs.size() == 1 ? this->lhsArgs.front() : this->rhsArgs.front();
	ClauseArgument synonymArg = this->lhsArgs.size() == 2 ? this->lhsArgs.front() : this->rhsArgs.front();
	ClauseArgument attributeArg = this->lhsArgs.size() == 2 ? this->lhsArgs.back() : this->rhsArgs.back();

	assert(synonymArg.isSynonym() && attributeArg.isAttributeName() && exactArg.isExactReference());

	if (synonymArg.isReadSynonym() && attributeArg.isVarNameAttribute()) {
		return RelationshipClauseResult(synonymArg, exactArg, 
			pkb->retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType::MODIFIES, synonymArg, exactArg));
	}
	else if (synonymArg.isPrintSynonym() && attributeArg.isVarNameAttribute()) {
		return RelationshipClauseResult(synonymArg, exactArg, 
			pkb->retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType::USES, synonymArg, exactArg));
	}
	else if (synonymArg.isCallSynonym() && attributeArg.isProcNameAttribute()) {
		// blocked
		throw PQLLogicError("Not supported yet");
	}

	PQLEntity retrievedEntity = this->retrieveEntityByExactArg(pkb, synonymArg, exactArg);
	PQLRelationship relationship = PQLRelationship(retrievedEntity, retrievedEntity);
	return RelationshipClauseResult(synonymArg, exactArg, vector<PQLRelationship>{relationship});
}

RelationshipClauseResult WithClause::executeNoExact(shared_ptr<PKBQueryHandler> pkb) {
	assert(this->lhsArgs.size() == 2 && this->rhsArgs.size() == 2);

	throw PQLLogicError("Not supported yet");
}

PQLEntity WithClause::retrieveEntityByExactArg(shared_ptr<PKBQueryHandler> pkb, ClauseArgument& synonymArg, ClauseArgument& exactArg) {
	if (synonymArg.isProcedureSynonym()) {
		return pkb->retrieveProcedureEntityByName(exactArg.getIdentifier());
	}
	else if (synonymArg.isStmtRefNoWildcard()) {
		PKBTrackedStatementType stmtType = this->getPKBStmtType(synonymArg);
		return pkb->retrieveStatementEntityByLineNumber(exactArg.getLineNumber(), stmtType);
	}
	else if (synonymArg.isVariableSynonym()) {
		return pkb->retrieveVariableByName(exactArg.getIdentifier());
	}
	else if (synonymArg.isConstantSynonym()) {
		return pkb->retrieveConstantByValue(exactArg.getLineNumber());
	}
	else {
		throw PQLLogicError("Cannot identify entity type in with clause synonym");
	}
}

PKBTrackedStatementType WithClause::getPKBStmtType(ClauseArgument& stmtSynonymArg) {
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

bool WithClause::equals(shared_ptr<WithClause> other) {
	return this->lhsArgs == other->lhsArgs && this->rhsArgs == other-> rhsArgs;
}