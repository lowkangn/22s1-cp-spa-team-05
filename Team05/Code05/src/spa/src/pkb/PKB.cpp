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
	this->repository = this->updateManager.addEntities(entities, this->repository);
}


void PKB::addRelationships(vector<Relationship> relationships) {
	this->repository = this->updateManager.addRelationships(relationships, this->repository);
}


void PKB::addPatterns(vector<Pattern> patterns) {
	this->repository = this->updateManager.addPatterns(patterns, this->repository);
}


void PKB::addCfg(shared_ptr<CFGNode> rootNode) {
	this->repository = this->updateManager.addCfg(rootNode, this->repository);
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
	return this->queryManager.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs, this->repository);
}



vector<PQLRelationship> PKB::retrieveRelationshipsFromTablesByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
	// 0. get table based on type
	shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(relationshipType);

	// 1. short circuiting
	if (canShortCircuitRetrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs)) {
		return vector<PQLRelationship>();
	}

	// 2. if both side are exact, we can search by hash
	// we create the key we are looking for based on lhs and rhs 
	if (lhs.isExactReference() && rhs.isExactReference()) {
		
		// 2.1 create relationship
		shared_ptr<PkbEntity> left = this->convertClauseArgumentToPkbEntity(lhs);
		shared_ptr<PkbEntity> right = this->convertClauseArgumentToPkbEntity(rhs);
		shared_ptr<PkbRelationship> toFind; // TODO: refactor to use factory methods
		switch (relationshipType) {
		case PKBTrackedRelationshipType::FOLLOWS:
			toFind = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::FOLLOWSSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::PARENT:
			toFind = shared_ptr<PkbRelationship>(new PkbParentRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::PARENTSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::USES:
			toFind = shared_ptr<PkbRelationship>(new PkbUsesRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::MODIFIES:
			toFind = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(left, right));
			break;
		case  PKBTrackedRelationshipType::NEXT:
			toFind = shared_ptr<PkbRelationship>(new PkbNextRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::CALLSTMTATTRIBUTE:
			toFind = shared_ptr<PkbRelationship>(new PkbCallStmtAttributeRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::CALLS:
			toFind = shared_ptr<PkbRelationship>(new PkbCallsRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::CALLSSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(left, right));
			break;
		default:
			throw PkbException("Unknown relationship type to be retrieved!");
		}

		// 2.2 get by key
		string key = toFind->getKey();
		shared_ptr<PkbRelationship> found = table->get(key);


		// 2.3 if null, return empty 
		vector<PQLRelationship> retrievedRelationships;
		if (found == NULL) {
			return retrievedRelationships;
		}

		// 2.4 if not null, convert and return in single item vector
		PQLEntity outLhs = this->pkbEntityToQpsPqlEntity(found->getLhs());
		PQLEntity outRhs = this->pkbEntityToQpsPqlEntity(found->getRhs());
		retrievedRelationships.push_back(PQLRelationship(outLhs, outRhs));
		return retrievedRelationships;
	}
	

	// 3. if not, we have to manually filter
	PkbEntityFilter lhsFilter = getFilterFromClauseArgument(lhs);
	PkbEntityFilter rhsFilter = getFilterFromClauseArgument(rhs);
	vector<PQLRelationship> out;
	for (shared_ptr<PkbRelationship> r : table->getAll()) {
		shared_ptr<PkbEntity> lhsEntity = r->getLhs();
		shared_ptr<PkbEntity> rhsEntity = r->getRhs();

		
		if (lhsFilter(lhsEntity, lhs) && rhsFilter(rhsEntity, rhs)) {
			// create pql relationship
			PQLEntity outLhs = this->pkbEntityToQpsPqlEntity(lhsEntity);
			PQLEntity outRhs = this->pkbEntityToQpsPqlEntity(rhsEntity);
			out.push_back(PQLRelationship(outLhs, outRhs));
		}
	}
	return out;
}




shared_ptr<PKBQueryHandler> PKB::getQueryHandler() {
	return shared_ptr<PKBQueryHandler>(this);
}

shared_ptr<PKBUpdateHandler> PKB::getUpdateHandler() {
	return shared_ptr<PKBUpdateHandler>(this);
}

vector<PQLPattern> PKB::retrievePatterns(PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs) {
	
	if (statementType == PKBTrackedStatementType::ASSIGN) {
		return this->retrieveAssignPatterns(lhs, rhs);
		
	}
	else if (statementType == PKBTrackedStatementType::IF) {
		assert(rhs.isWildcard());
		return this->retrieveIfPatterns(lhs);
	}
	else if (statementType == PKBTrackedStatementType::WHILE) {
		assert(rhs.isWildcard());
		return this->retrieveWhilePatterns(lhs);
	}
	else {
		throw PkbException("Unknown pattern type to be retrieved!");
	}
}


vector<PQLPattern> PKB::retrieveAssignPatterns(ClauseArgument lhs, ClauseArgument rhs) {
	// 1. parse lhs 
	string lhsStringPattern;

	bool lhsIsSynonym = false;
	if (lhs.isVariableSynonym() || lhs.isWildcard()) {
		// if lhs is a synonym, we treat as a wildcard, but flag it - we will deal with it later
		lhsIsSynonym = lhs.isVariableSynonym();
		// in either case, we match lhs with a wildcard
		lhsStringPattern = WILDCARD_CHAR;
	}
	else if (lhs.isStringLiteral() || lhs.isPatternString() || lhs.isPatternStringWithWildcards()) {
		// string literal, we match exactly
		lhsStringPattern = lhs.getIdentifier();
	}
	else {
		// should never get here
		throw PkbException("Unknown assign pattern being retrieved!");
	}

	// 2. parse rhs - it should only be wildcard or string literal
	string rhsStringPattern;
	if (rhs.isWildcard()) {
		rhsStringPattern = WILDCARD_CHAR;
	}
	else if (rhs.isPatternString() || rhs.isPatternStringWithWildcards()) {
		rhsStringPattern = rhs.getIdentifier();
	}
	else {
		throw PkbException("RHS of assign pattern should be wildcard, sandwiched wildcard or string, but is neither.");
	}

	// 3. find all patterns that match in the assign table
	vector<string> postFixStringsToMatch = {
		lhsStringPattern, rhsStringPattern
	};
	vector<shared_ptr<PkbPattern>> matchingPatterns = this->assignPatterns.getAllThatMatchPostFixStrings(postFixStringsToMatch);


	// 5. for each returned statement, we get the corr. entity
	vector<PQLPattern> out;
	for (shared_ptr<PkbPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}

vector<PQLPattern> PKB::retrieveIfPatterns(ClauseArgument lhs) {
	// parse lhs 
	string lhsStringPattern;

	bool lhsIsSynonym = false;
	if (lhs.isVariableSynonym() || lhs.isWildcard()) {
		// if lhs is synonym, flag as wildcard
		lhsIsSynonym = lhs.isVariableSynonym();
		lhsStringPattern = WILDCARD_CHAR;
	}
	else if (lhs.isStringLiteral()) {
		// string literal, we match exactly
		lhsStringPattern = lhs.getIdentifier();
	}
	else {
		throw PkbException("Unknown if pattern being retrieved!");
	}


	vector<shared_ptr<PkbPattern>> matchingPatterns = this->ifPatterns.getVariableMatch(lhsStringPattern);

	vector<PQLPattern> out;
	for (shared_ptr<PkbPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}

vector<PQLPattern> PKB::retrieveWhilePatterns(ClauseArgument lhs) {
	// parse lhs 
	string lhsStringPattern;

	bool lhsIsSynonym = false;
	if (lhs.isVariableSynonym() || lhs.isWildcard()) {
		// if lhs is synonym, flag as wildcard
		lhsIsSynonym = lhs.isVariableSynonym();
		lhsStringPattern = WILDCARD_CHAR;
	}
	else if (lhs.isStringLiteral()) {
		// string literal, we match exactly
		lhsStringPattern = lhs.getIdentifier();
	}
	else {
		throw PkbException("Unknown if pattern being retrieved!");
	}

	vector<shared_ptr<PkbPattern>> matchingPatterns = this->whilePatterns.getVariableMatch(lhsStringPattern);

	vector<PQLPattern> out;
	for (shared_ptr<PkbPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}



