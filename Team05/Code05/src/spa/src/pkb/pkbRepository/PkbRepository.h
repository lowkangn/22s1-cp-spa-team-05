
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
	shared_ptr<PkbGraphManager> cfgManager = shared_ptr<PkbGraphManager>();

	// ======================== Helper methods ==============================
	

public: 
	// ==================== Setters ====================
	void addPkbEntity(shared_ptr<PkbEntity> e) {
		// depending on type, we add to the correct table
		if (e->isConstant()) {
			this->constantsTable->add(e);
		}
		else if (e->isProcedure()) {
			this->proceduresTable->add(e);
		}
		else if (e->isVariable()) {
			this->variableTable->add(e);
		}
		else if (e->isStatement()) {
			this->statementTable->add(e);
		}
		else {
			throw PkbException("Unknown pkb pattern being added to repository.");
		}
	}

	void addPkbRelationship(shared_ptr<PkbRelationship> r) {
		if (r->isFollows()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::FOLLOWS);
			table->add(r);
		}
		else if (r->isFollowsStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::FOLLOWSSTAR);
			table->add(r);
		}
		else if (r->isParent()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::PARENT);
			table->add(r);
		}
		else if (r->isParentStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::PARENTSTAR);
			table->add(r);
		}
		else if (r->isUses()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::USES);
			table->add(r);
		}
		else if (r->isModifies()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::MODIFIES);
			table->add(r);
		}
		else if (r->isCallStmtAttribute()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::CALLSTMTATTRIBUTE);
			table->add(r);
		}
		else if (r->isCalls()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::CALLS);
			table->add(r);
		}
		else if (r->isCallsStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::CALLSSTAR);
			table->add(r);
		}
		else if (r->isNext()) {
			shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(PkbRelationshipType::NEXT);
			table->add(r);
		}
		else {
			throw PkbException("Unknown relationship being added to repository!");
		}
	}

	void addPkbPattern(shared_ptr<PkbPattern> p) {
		// depending on type, we add to the correct table
		if (p->isAssignPattern()) {
			this->assignPatterns->add(p);
		}
		else if (p->isWhilePattern()) {
			this->whilePatterns->add(p);
		}
		else if (p->isIfPattern()) {
			this->ifPatterns->add(p);
		}
		else {
			throw PkbException("Unknown pkb pattern being added to repository.");
		}
	}

	void addPkbGraph(shared_ptr<PkbGraphNode> cfg) {
		this->cfgManager = shared_ptr<PkbGraphManager>(new PkbGraphManager(cfg));
	}


	// ==================== Getters ====================

	/*
		Maps the supported relationship types to an internal table.
	*/
	shared_ptr<PkbRelationshipTable> getTableByRelationshipType(PkbRelationshipType relationshipType) {
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
			throw PkbException("Unknown pattern type to be retrieved!");
		}
	}

	shared_ptr<PkbGraphManager> getCfg() {
		return this->cfgManager;
	}


	

};