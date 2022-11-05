#include <pkb/PKB.h>
#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbConstantEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbWhilePattern.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallStmtAttributeRelationship.h>
#include <qps/query/clause/PQLEntity.h>
#include <StringSplitter.h>

#include <iostream>

using namespace std;

// ==================== Private methods ====================



// ==================== Public methods ====================

// ******************** update ********************

void PKB::addEntities(vector<Entity> entities) {
	this->updateManager.addEntities(entities, this->repository);
}


void PKB::addRelationships(vector<Relationship> relationships) {
	this->updateManager.addRelationships(relationships, this->repository);
}


void PKB::addPatterns(vector<Pattern> patterns) {
	this->updateManager.addPatterns(patterns, this->repository);
}


void PKB::addCfgs(vector<shared_ptr<CFGNode>> rootNodes) {
	this->updateManager.addCfgs(rootNodes, this->repository);
}


// ******************** retrieval ********************

// -------------------- all --------------------
vector<PQLEntity> PKB::retrieveAllProcedureEntities() {
	return this->queryManager.retrieveAllProcedureEntities(this->repository);
}

vector<PQLEntity> PKB::retrieveAllStatementEntities() {
	return this->queryManager.retrieveAllStatementEntities(this->repository);
}

vector<PQLEntity> PKB::retrieveAllConstants() {
	return this->queryManager.retrieveAllConstants(this->repository);
}

vector<PQLEntity> PKB::retrieveAllVariables() {
	return this->queryManager.retrieveAllVariables(this->repository);
}


// -------------------- by type --------------------
vector<PQLEntity> PKB::retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) {
	
	return this->queryManager.retrieveStatementEntitiesByType(pkbTrackedStatementType, this->repository);
}

vector<PQLRelationship> PKB::retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType) {
	return this->queryManager.retrieveRelationshipsByType(relationshipType, this->repository);
}

// -------------------- by identifier --------------------
optional<PQLEntity> PKB::retrieveProcedureEntityByName(string procedureName) {
	return this->queryManager.retrieveProcedureEntityByName(procedureName, this->repository);
}


optional<PQLEntity> PKB::retrieveStatementByLineNumberAndType(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) {
	return this->queryManager.retrieveStatementByLineNumberAndType(lineNumber, pkbTrackedStatementType, this->repository);
}

optional<PQLEntity> PKB::retrieveVariableByName(string name) {
	return this->queryManager.retrieveVariableByName(name, this->repository);
}


optional<PQLEntity> PKB::retrieveConstantByValue(int value) {
	return this->queryManager.retrieveConstantByValue(value, this->repository);
}

// -------------------- specifics --------------------
vector<PQLRelationship> PKB::retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
	return this->queryManager.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs, this->repository, this->optimized);
}


vector<PQLPattern> PKB::retrievePatterns(PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs) {
	return this->queryManager.retrievePatterns(statementType, lhs, rhs, this->repository);
	
}





