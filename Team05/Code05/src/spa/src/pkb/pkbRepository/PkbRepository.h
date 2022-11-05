
#pragma once

#include <pkb/pkbRepository/table_managers/PkbEntityTable.h>
#include <pkb/pkbRepository/table_managers/PkbRelationshipTable.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/pkbRepository/graph_managers/PkbGraphManager.h>

#include <map>

using namespace std;

const string FOLLOWS_TABLE = "follows";
const string FOLLOWSSTAR_TABLE = "followsStar";
const string PARENT_TABLE = "parent";
const string PARENTSTAR_TABLE = "parentStar";
const string USES_TABLE = "uses";
const string MODIFIES_TABLE = "modifies";
const string NEXT_TABLE = "next";
const string NEXTSTAR_TABLE = "nextStar";
const string NOT_NEXTSTAR_TABLE = "notNextStar";
const string CALLS_ATTRIBUTE_TABLE = "callsAttribute";
const string CALLS_TABLE = "calls";
const string CALLSSTAR_TABLE = "callsStar";
const string AFFECTS_TABLE = "affects";
const string NOT_AFFECTS_TABLE = "notAffects";
const string AFFECTSSTAR_TABLE = "affectsStar";
const string NOT_AFFECTSSTAR_TABLE = "notAffectsStar";


/*
	In charge of storing all objects.
*/
class PkbRepository {
private: 

	// ==================== Attributes ====================
	// variables, statement and procedures
	shared_ptr<PkbEntityTable> variableTable = shared_ptr<PkbEntityTable>(new PkbEntityTable());
	shared_ptr<PkbEntityTable> statementTable = shared_ptr<PkbEntityTable>(new PkbEntityTable());
	shared_ptr<PkbEntityTable> proceduresTable = shared_ptr<PkbEntityTable>(new PkbEntityTable());
	shared_ptr<PkbEntityTable> constantsTable = shared_ptr<PkbEntityTable>(new PkbEntityTable());

	// relationships
	map<string, shared_ptr<PkbRelationshipTable>> relationshipTables{
		{FOLLOWS_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{FOLLOWSSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{PARENT_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{PARENTSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{USES_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{MODIFIES_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{NEXT_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{CALLS_ATTRIBUTE_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{CALLS_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{CALLSSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{AFFECTS_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())}, // as caches
		{AFFECTSSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())}, // as caches
		{NOT_AFFECTS_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())}, // as caches
		{NOT_AFFECTSSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())}, // as caches
		{NEXTSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())}, // as caches
		{NOT_NEXTSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())}, // as caches
	};

	// patterns
	shared_ptr<PkbPatternTable> assignPatterns = shared_ptr<PkbPatternTable>(new PkbPatternTable());
	shared_ptr<PkbPatternTable> ifPatterns = shared_ptr<PkbPatternTable>(new PkbPatternTable());;
	shared_ptr<PkbPatternTable> whilePatterns = shared_ptr<PkbPatternTable>(new PkbPatternTable());;

	// graphs
	vector<shared_ptr<PkbGraphManager>> cfgManagers;

	// other data
	vector<int> procedureRootStatementNos;

	// ======================== Helper methods ==============================
	

public: 
	// ==================== Setters ====================
	void addPkbEntity(shared_ptr<PkbEntity> entity) {
		// depending on type, we add to the correct table
		if (entity->isConstant()) {
			this->constantsTable->add(entity);
		}
		else if (entity->isProcedure()) {
			this->proceduresTable->add(entity);
		}
		else if (entity->isVariable()) {
			this->variableTable->add(entity);
		}
		else if (entity->isStatement()) {
			this->statementTable->add(entity);
		}
		else {
			throw PkbException("Unknown pkb pattern being added to repository.");
		}
	}

	void addPkbRelationship(shared_ptr<PkbRelationship> relationship) {
		if (relationship->isFollows()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::FOLLOWS);
			table->add(relationship);
		}
		else if (relationship->isFollowsStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::FOLLOWSSTAR);
			table->add(relationship);
		}
		else if (relationship->isParent()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::PARENT);
			table->add(relationship);
		}
		else if (relationship->isParentStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::PARENTSTAR);
			table->add(relationship);
		}
		else if (relationship->isUses()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::USES);
			table->add(relationship);
		}
		else if (relationship->isModifies()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::MODIFIES);
			table->add(relationship);
		}
		else if (relationship->isCallStmtAttribute()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::CALLSTMTATTRIBUTE);
			table->add(relationship);
		}
		else if (relationship->isCalls()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::CALLS);
			table->add(relationship);
		}
		else if (relationship->isCallsStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::CALLSSTAR);
			table->add(relationship);
		}
		else if (relationship->isNext()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::NEXT);
			table->add(relationship);
		}
		else if (relationship->isNextStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::NEXTSTAR);
			table->add(relationship);
		}
		else if (relationship->isNextStarNot()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_NEXTSTAR);
			table->add(relationship);
		}
		else if (relationship->isAffects()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTS);
			table->add(relationship);
		}
		else if (relationship->isAffectsStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTSSTAR);
			table->add(relationship);
		}
		else if (relationship->isAffectsNot()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTS);
			table->add(relationship);
		}
		else if (relationship->isAffectsStarNot()) {
			shared_ptr<PkbRelationshipTable> table = this->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTSSTAR);
			table->add(relationship);
		}
		else {
			throw PkbException("Unknown relationship being added to repository!");
		}
	}

	void addPkbPattern(shared_ptr<PkbPattern> pattern) {
		// depending on type, we add to the correct table
		if (pattern->isAssignPattern()) {
			this->assignPatterns->add(pattern);
		}
		else if (pattern->isWhilePattern()) {
			this->whilePatterns->add(pattern);
		}
		else if (pattern->isIfPattern()) {
			this->ifPatterns->add(pattern);
		}
		else {
			throw PkbException("Unknown pkb pattern being added to repository.");
		}
	}

	void addPkbGraph(shared_ptr<PkbGraphNode> cfg) {
		shared_ptr<PkbGraphManager> graphManager = make_shared<PkbGraphManager>(cfg);
		this->cfgManagers.push_back(graphManager);
		this->procedureRootStatementNos.push_back(graphManager->getRootNode()->getAsEntity()->getLineNumber());

	}


	// ==================== Getters ====================

	/*
		Maps the supported relationship types to an internal table.
	*/
	shared_ptr<PkbRelationshipTable> getRelationshipTableByRelationshipType(PkbRelationshipType relationshipType) {
		switch (relationshipType) {
		case PkbRelationshipType::FOLLOWS:
			return this->relationshipTables[FOLLOWS_TABLE];
		case PkbRelationshipType::FOLLOWSSTAR:
			return this->relationshipTables[FOLLOWSSTAR_TABLE];
		case PkbRelationshipType::PARENT:
			return  this->relationshipTables[PARENT_TABLE];
		case PkbRelationshipType::PARENTSTAR:
			return this->relationshipTables[PARENTSTAR_TABLE];
		case PkbRelationshipType::USES:
			return this->relationshipTables[USES_TABLE];
		case PkbRelationshipType::MODIFIES:
			return this->relationshipTables[MODIFIES_TABLE];
		case PkbRelationshipType::NEXT:
			return this->relationshipTables[NEXT_TABLE];
		case PkbRelationshipType::NEXTSTAR:
			return this->relationshipTables[NEXTSTAR_TABLE];
		case PkbRelationshipType::NOT_NEXTSTAR:
			return this->relationshipTables[NOT_NEXTSTAR_TABLE];
		case PkbRelationshipType::CALLSTMTATTRIBUTE:
			return this->relationshipTables[CALLS_ATTRIBUTE_TABLE];
		case PkbRelationshipType::CALLS:
			return this->relationshipTables[CALLS_TABLE];
		case PkbRelationshipType::CALLSSTAR:
			return this->relationshipTables[CALLSSTAR_TABLE];
		case PkbRelationshipType::AFFECTS:
			return this->relationshipTables[AFFECTS_TABLE];
		case PkbRelationshipType::AFFECTSSTAR:
			return this->relationshipTables[AFFECTSSTAR_TABLE];
		case PkbRelationshipType::NOT_AFFECTS:
			return this->relationshipTables[NOT_AFFECTS_TABLE];
		case PkbRelationshipType::NOT_AFFECTSSTAR:
			return this->relationshipTables[NOT_AFFECTSSTAR_TABLE];
		default:
			throw PkbException("Unknown relationship type to be retrieved!");
		}
	}

	/*
		Maps the supported entity types to an internal table.
	*/
	shared_ptr<PkbEntityTable> getEntityTableByType(PkbEntityType entityType) {
		switch (entityType) {
		case PkbEntityType::CONSTANT:
			return this->constantsTable;
		case PkbEntityType::PROCEDURE:
			return this->proceduresTable;
		case PkbEntityType::STATEMENT:
			return this->statementTable;
		case PkbEntityType::VARIABLE:
			return this->variableTable;
		default:
			throw PkbException("Unknown entity type to be retrieved!");
		}
	}

	/*
		Maps the supported pattern types to an internal table.
	*/
	shared_ptr<PkbPatternTable> getPatternTableByType(PkbPatternType patternType) {
		switch (patternType) {
		case PkbPatternType::ASSIGN:
			return this->assignPatterns;
		case PkbPatternType::IF:
			return this->ifPatterns;
		case PkbPatternType::WHILE:
			return this->whilePatterns;
		default:
			throw PkbException("Unknown pattern table to be retrieved!");
		}
	}
	
	/*
		Gets all stored cfgs.
	*/
	vector<shared_ptr<PkbGraphManager>> getCfgs() {
		return this->cfgManagers;
	}

	/*
		Checks whether statements are in the same procedure.
		NOTE: this is a bit hacky, but for now, we keep it here.
	*/
	bool statementsAreInTheSameProcedure(int lineNumber1, int lineNumber2) {
		int oneBelongsToProcedure;
		int twoBelongsToProcedure;
		for (int rootLineNumber : this->procedureRootStatementNos) {
			// check line number 1
			if (lineNumber1 >= rootLineNumber) {
				oneBelongsToProcedure = rootLineNumber;
			}
			if (lineNumber2 >= rootLineNumber) {
				twoBelongsToProcedure = rootLineNumber;
			}
		}
		return oneBelongsToProcedure == twoBelongsToProcedure;
	}

	// ==================== State managers ====================
	/*
		Resets cached queries.
	*/
	void resetCache() {
		this->relationshipTables[NEXTSTAR_TABLE] = make_shared<PkbRelationshipTable>();
		this->relationshipTables[NOT_NEXTSTAR_TABLE] = make_shared<PkbRelationshipTable>();
		this->relationshipTables[AFFECTSSTAR_TABLE] = make_shared<PkbRelationshipTable>();
		this->relationshipTables[NOT_AFFECTSSTAR_TABLE] = make_shared<PkbRelationshipTable>();
		this->relationshipTables[AFFECTS_TABLE] = make_shared<PkbRelationshipTable>();
		this->relationshipTables[NOT_AFFECTS_TABLE] = make_shared<PkbRelationshipTable>();
	}

};