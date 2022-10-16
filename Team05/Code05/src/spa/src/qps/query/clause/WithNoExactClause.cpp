#include <qps/query/clause/WithNoExactClause.h>

shared_ptr<ClauseResult> WithNoExactClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	if (this->lhsSynonym == this->rhsSynonym && this->lhsAttribute == this->rhsAttribute) {
		return this->handleBothSidesEqual();
	} 
	else if (this->lhsAttribute.isStmtNumAttribute() && this->rhsAttribute.isStmtNumAttribute()) {
		return this->handleBothSidesStmtNumAttribute(pkb);
	} 

	// Evaluate each side of the with clause
	shared_ptr<ClauseResult> lhsResult = this->getSingleAttrRefResult(pkb, this->lhsSynonym, this->lhsAttribute);
	shared_ptr<ClauseResult> rhsResult = this->getSingleAttrRefResult(pkb, this->rhsSynonym, this->rhsAttribute);
	
	// Evaluate the '='
	ClauseArgument leftOn = this->getEqualityCheckColumn(this->lhsSynonym, this->lhsAttribute);
	ClauseArgument rightOn = this->getEqualityCheckColumn(this->rhsSynonym, this->rhsAttribute);
	ClauseResult finalResult = lhsResult->mergeByForceInnerJoin(*rhsResult, leftOn, rightOn);
	return make_shared<ClauseResult>(finalResult);
}

shared_ptr<EntityClauseResult> WithNoExactClause::handleBothSidesEqual() {
	//both sides are exactly equal, so this with clause does not add any constraint to the final result
	return make_shared<EntityClauseResult>(EntityClauseResult::createNonEmptyNoSynonymResult());
}

shared_ptr<ClauseResult> WithNoExactClause::handleBothSidesStmtNumAttribute(shared_ptr<PKBQueryHandler> pkb) {
	assert(lhsAttribute.isStmtNumAttribute() && rhsAttribute.isStmtNumAttribute());
	assert(this->lhsSynonym != this->rhsSynonym);

	if (!this->lhsSynonym.isSameType(this->rhsSynonym) && !this->lhsSynonym.isStmtSynonym() 
		&& !this->rhsSynonym.isStmtSynonym()) {
		//two different statement types cannot have the same statement number
		return make_shared<EntityClauseResult>(EntityClauseResult::createEmptyNoSynonymResult());
	}

	// If both sides are the same type, we only need to retrieve either side. If one side is a stmt synonym, it suffices to retrieve the other side.
	ClauseArgument synonymToGet = lhsSynonym;
	ClauseArgument otherSynonym = rhsSynonym;
	if (this->lhsSynonym.isStmtSynonym()) {
		synonymToGet = rhsSynonym;
		otherSynonym = lhsSynonym;
	}
	PKBTrackedStatementType statementType = this->getPKBStmtType(synonymToGet);
	vector<PQLEntity> retrievedEntities = pkb->retrieveStatementEntitiesByType(statementType);

	EntityClauseResult synonymToGetResult = EntityClauseResult(synonymToGet, retrievedEntities);
	EntityClauseResult otherSynonymResult = EntityClauseResult(otherSynonym, retrievedEntities);
	ClauseResult finalResult = synonymToGetResult.mergeByForceInnerJoin(otherSynonymResult, synonymToGet, otherSynonym);
	return make_shared<ClauseResult>(finalResult);
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