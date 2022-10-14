#include <qps/query/clause/AttributeClause.h>

shared_ptr<ClauseResult> AttributeClause::getSingleAttrRefResult(shared_ptr<PKBQueryHandler> pkb,
	ClauseArgument& synonym, ClauseArgument& attribute) {
	if (synonym.isProcedureSynonym()) {
		return make_shared<EntityClauseResult>(synonym, pkb->retrieveAllProcedureEntities());
	}
	else if (synonym.isConstantSynonym()) {
		return make_shared<EntityClauseResult>(synonym, pkb->retrieveAllConstants());
	}
	else if (synonym.isVariableSynonym()) {
		return make_shared<EntityClauseResult>(synonym, pkb->retrieveAllVariables());
	}
	else if (synonym.isStmtRefNoWildcard() && attribute.isStmtNumAttribute()) {
		PKBTrackedStatementType statementType = this->getPKBStmtType(synonym);
		return make_shared<EntityClauseResult>(synonym, pkb->retrieveStatementEntitiesByType(statementType));
	}
	else {
		//non-default attributes, modelled as relationships
		PKBTrackedRelationshipType relationshipToRetrieve;
		if (synonym.isCallSynonym() && attribute.isProcNameAttribute()) {
			relationshipToRetrieve = PKBTrackedRelationshipType::CALLSTMTATTRIBUTE;
		}
		else if (synonym.isReadSynonym() && attribute.isVarNameAttribute()) {
			relationshipToRetrieve = PKBTrackedRelationshipType::MODIFIES;
		}
		else if (synonym.isPrintSynonym() && attribute.isVarNameAttribute()) {
			relationshipToRetrieve = PKBTrackedRelationshipType::USES;
		}
		else {
			throw PQLLogicError("Unkown atrrRef being executed");
		}
		ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
		return make_shared<RelationshipClauseResult>(synonym, attribute,
			pkb->retrieveRelationshipByTypeAndLhsRhs(relationshipToRetrieve, synonym, wildcardArg));
	}
}

PKBTrackedStatementType AttributeClause::getPKBStmtType(ClauseArgument& stmtSynonymArg) {
	assert(stmtSynonymArg.isStmtRefNoWildcard() && stmtSynonymArg.isSynonym());
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