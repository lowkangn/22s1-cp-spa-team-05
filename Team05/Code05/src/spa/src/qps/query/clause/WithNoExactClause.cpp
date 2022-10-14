#include <qps/query/clause/WithNoExactClause.h>

shared_ptr<ClauseResult> WithNoExactClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	if (this->lhsSynonym == this->rhsSynonym && this->lhsAttribute == this->rhsAttribute) {
		return this->handleBothSidesEqual();
	} 
	else if (this->lhsSynonym.isStmtRefNoWildcard() && this->rhsSynonym.isStmtRefNoWildcard()) {
		return this->handleBothSidesStmtNumAttribute(pkb);
	} 

	// Evaluate each side of the with clause
	shared_ptr<ClauseResult> lhsResult = this->retrieveForOneAttrRef(pkb, this->lhsSynonym, this->lhsAttribute);
	shared_ptr<ClauseResult> rhsResult = this->retrieveForOneAttrRef(pkb, this->rhsSynonym, this->rhsAttribute);
	
	// Evaluate the '='
	ClauseArgument leftOn = this->getEqualityCheckColumn(this->lhsSynonym, this->lhsAttribute);
	ClauseArgument rightOn = this->getEqualityCheckColumn(this->rhsSynonym, this->rhsAttribute);
	//ClauseResult finalResult = lhsResult->mergeByForceInnerJoin(*rhsResult, leftOn, rightOn);
	//return make_shared<ClauseResult>(finalResult);

	return make_shared<RelationshipClauseResult>(lhsSynonym, rhsSynonym, vector<PQLRelationship>{});
}

shared_ptr<EntityClauseResult> WithNoExactClause::handleBothSidesEqual() {
	return make_shared<EntityClauseResult>(EntityClauseResult::createNonEmptyNoSynonymResult());
}

shared_ptr<ClauseResult> WithNoExactClause::handleBothSidesStmtNumAttribute(shared_ptr<PKBQueryHandler> pkb) {
	assert(lhsAttribute.isStmtNumAttribute() && rhsAttribute.isStmtNumAttribute());
	assert(this->lhsSynonym != this->rhsSynonym); // == case should be handled by handleBothSidesEqual

	if (!this->lhsSynonym.isSameType(this->rhsSynonym) && !this->lhsSynonym.isStmtSynonym() 
		&& !this->rhsSynonym.isStmtSynonym()) {
		//two different statement types cannot have the same statement number
		return make_shared<EntityClauseResult>(EntityClauseResult::createEmptyNoSynonymResult());
	}

	
	ClauseArgument synonymToGet = lhsSynonym;
	ClauseArgument otherSynonym = lhsSynonym;

	// If both sides are the same type, we only need to retrieve either side. If one side is a stmt synonym, it suffices to retrieve the other side.
	if (this->lhsSynonym.isSameType(this->rhsSynonym) || this->lhsSynonym.isStmtSynonym()) {
		synonymToGet = rhsSynonym;
		otherSynonym = lhsSynonym;
	}
	else {
		synonymToGet = lhsSynonym;
		otherSynonym = rhsSynonym;
	}
	PKBTrackedStatementType statementType = this->getPKBStmtType(synonymToGet);
	vector<PQLEntity> retrievedEntities = pkb->retrieveStatementEntitiesByType(statementType);

	EntityClauseResult synonymToGetResult = EntityClauseResult(synonymToGet, retrievedEntities);
	EntityClauseResult stmtSynonymResult = EntityClauseResult(otherSynonym, retrievedEntities);
	//ClauseResult finalResult = synonymToGetResult.mergeByForceInnerJoin(stmtSynonymResult, synonymToGet, otherSynonym);
	//return make_shared<ClauseResult>(finalResult);
	
	/*
	* 
	Merges this ClauseResult with another ClauseResult by performing an inner join on the given join columns.
	Throws PQLLogicError if a join column is not present in either ClauseResult.
	ClauseResult mergeByForceInnerJoin(ClauseResult resultToMerge, ClauseArgument leftOn, ClauseArgument rightOn) {
		bool leftHasArg = find(this->args.begin(), this->args.end(), leftOn) == this->args.end();
		bool rightHasArg = find(resultToMerge.args.begin(), resultToMerge.args.end(), rightOn) == resultToMerge.args.end();
		if (!leftHasArg || !rightHasArg) {
			throw PQLLogicError("ClauseResults must have the columns to force inner join on");
		}
		resultToMerge.duplicateExistingColumnAs(rightOn, leftOn);
		return this->performInnerJoin(resultToMerge, vector<ClauseArgument>{ leftOn });
	}

	//PROTECTED

	Duplicates an existing column of this ClauseResult and renames it to the newColumnHeader.
	ClauseResult duplicateExistingColumnAs(ClauseArgument headerOfColumnToDuplicate, ClauseArgument newColumnHeader) {
		vector<int> indices = this->getColumnIndices(vector<ClauseArgument>{ headerOfColumnToDuplicate });
		assert(indices.size() == 1 && indices.front() != -1); //column must be an existing column
		
		ClauseResult columnToDuplicate = this->getColumn(indices.front());
		columnToDuplicate.renameColumn(headerOfColumnToDuplicate, newColumnHeader);
		this->addColumn(columnToDuplicate);
	}

	Renames columns currently named oldName to newName
	void renameColumn(ClauseArgument oldName, ClauseArgument newName) {
		replace(args.begin(), args.end(), oldName, newName);
	}
	*/
	vector<PQLRelationship> entitiesDuplicated;
	for (PQLEntity entity : retrievedEntities) {
		entitiesDuplicated.push_back(PQLRelationship(entity, entity));
	}

	return make_shared<RelationshipClauseResult>(synonymToGet, otherSynonym, entitiesDuplicated);
}

shared_ptr<ClauseResult> WithNoExactClause::retrieveForOneAttrRef(shared_ptr<PKBQueryHandler> pkb,
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

ClauseArgument WithNoExactClause::getEqualityCheckColumn(ClauseArgument& synonym, ClauseArgument& attribute) {
	if (synonym.isProcedureSynonym() || synonym.isConstantSynonym() || synonym.isVariableSynonym()) {
		return synonym;
	}
	else if (synonym.isStmtRefNoWildcard() && attribute.isStmtNumAttribute()) {
		return synonym;
	}
	else {
		return attribute;
	}
}

bool WithNoExactClause::equals(shared_ptr<WithClause> other) {
	if (dynamic_pointer_cast<WithNoExactClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<WithNoExactClause> otherWithRelationship = dynamic_pointer_cast<WithNoExactClause>(other);
	return (this->lhsSynonym == otherWithRelationship->lhsSynonym) 
		&& (this->lhsAttribute == otherWithRelationship->lhsAttribute)
		&& (this->rhsSynonym == otherWithRelationship->rhsSynonym)
		&& (this->rhsAttribute == otherWithRelationship->rhsAttribute);
}