#include <qps/query/clause/WithOneExactClause.h>

shared_ptr<ClauseResult> WithOneExactClause::execute(shared_ptr<PKBQueryHandler> pkb) {
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
	else if (synonym.isStmtRefNoWildcard() && attribute.isStmtNumAttribute()) {
		PKBTrackedStatementType stmtType = this->getPKBStmtType(synonym);
		entity.emplace_back(pkb->retrieveStatementEntityByLineNumber(exactArg.getLineNumber(), stmtType));
	} 
	else if (synonym.isStmtRefNoWildcard() && (attribute.isProcNameAttribute () || attribute.isVarNameAttribute())) {
		return this->executeForStmtWithNameAttribute(pkb);
	}
	else {
		throw PQLLogicError("Unable to identify synonym type in with clause");
	}

	return make_shared<EntityClauseResult>(synonym, entity);
}

shared_ptr<ClauseResult> WithOneExactClause::executeForStmtWithNameAttribute(shared_ptr<PKBQueryHandler> pkb) {
	ClauseArgument synonym = this->nonExactArgs.front();
	ClauseArgument attribute = this->nonExactArgs.back();
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	vector<PQLRelationship> relationships;
	if (synonym.isPrintSynonym()) {
		relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType::USES, synonym, wildcardArg);
	}
	else if (synonym.isReadSynonym()) {
		relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType::MODIFIES, synonym, wildcardArg);
	}
	else if (synonym.isCallSynonym()) {
		//relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType::CALLSStatement, synonym, wildcardArg);
	}
	else {
		throw PQLLogicError("For statements, only print, read and call synonyms can have a name attribute.");
	}
	return make_shared<RelationshipClauseResult>(synonym, attribute, relationships);
}

bool WithOneExactClause::equals(shared_ptr<WithClause> other) {
	if (dynamic_pointer_cast<WithOneExactClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<WithOneExactClause> otherWithEntity = dynamic_pointer_cast<WithOneExactClause>(other);
	return (this->exactArg == otherWithEntity->exactArg) 
		&& (this->nonExactArgs == otherWithEntity->nonExactArgs);
}

PKBTrackedStatementType WithOneExactClause::getPKBStmtType(ClauseArgument& stmtSynonymArg) {
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