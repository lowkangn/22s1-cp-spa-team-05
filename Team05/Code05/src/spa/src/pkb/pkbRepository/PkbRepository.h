
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
const string CALLS_ATTRIBUTE_TABLE = "callsAttribute";
const string CALLS_TABLE = "calls";
const string CALLSSTAR_TABLE = "callsStar";

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
	};

	// patterns
	shared_ptr<PkbPatternTable> assignPatterns = shared_ptr<PkbPatternTable>(new PkbPatternTable());
	shared_ptr<PkbPatternTable> ifPatterns = shared_ptr<PkbPatternTable>(new PkbPatternTable());;
	shared_ptr<PkbPatternTable> whilePatterns = shared_ptr<PkbPatternTable>(new PkbPatternTable());;

	// graphs
	vector<shared_ptr<PkbGraphManager>> cfgManagers;

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
		this->cfgManagers.push_back(shared_ptr<PkbGraphManager>(new PkbGraphManager(cfg)));
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
		case PkbRelationshipType::CALLSTMTATTRIBUTE:
			return this->relationshipTables[CALLS_ATTRIBUTE_TABLE];
		case PkbRelationshipType::CALLS:
			return this->relationshipTables[CALLS_TABLE];
		case PkbRelationshipType::CALLSSTAR:
			return this->relationshipTables[CALLSSTAR_TABLE];
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

	vector<shared_ptr<PkbGraphManager>> getCfgs() {
		return this->cfgManagers;
	}

};