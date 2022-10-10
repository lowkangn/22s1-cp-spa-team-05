#pragma once

#include <pkb/interfaces/PKBQueryHandler.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLRelationship.h>

#include <pkb/table_managers/PkbEntityTable.h>
#include <pkb/table_managers/PkbRelationshipTable.h>
#include <pkb/table_managers/PkbPatternTable.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

const string FOLLOWS_TABLE = "follows";
const string FOLLOWSSTAR_TABLE = "followsStar";
const string PARENT_TABLE = "parent";
const string PARENTSTAR_TABLE = "parentStar";
const string USES_TABLE = "uses";
const string MODIFIES_TABLE = "modifies";

const string SPACE_DELIM = " ";

class PKB : public PKBQueryHandler, public PKBUpdateHandler {
private: 
	// variables, statement and procedures
	PkbEntityTable variableTable;
	PkbEntityTable statementTable;
	PkbEntityTable proceduresTable;
	PkbEntityTable constantsTable;

	// relationships
	map<string, shared_ptr<PkbRelationshipTable>> relationshipTables{
		{FOLLOWS_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{FOLLOWSSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{PARENT_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{PARENTSTAR_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{USES_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
		{MODIFIES_TABLE, shared_ptr<PkbRelationshipTable>(new PkbRelationshipTable())},
	};

	// patterns
	PkbPatternTable assignPatterns;
	PkbPatternTable ifPatterns;
	PkbPatternTable whilePatterns;

	// getters
	shared_ptr<PkbRelationshipTable> getFollowsTable() {
		return this->relationshipTables[FOLLOWS_TABLE];
	}
	shared_ptr<PkbRelationshipTable> getFollowsStarTable() {
		return this->relationshipTables[FOLLOWSSTAR_TABLE];
	}
	shared_ptr<PkbRelationshipTable> getParentTable() {
		return this->relationshipTables[PARENT_TABLE];
	}
	shared_ptr<PkbRelationshipTable> getParentStarTable() {
		return this->relationshipTables[PARENTSTAR_TABLE];
	}
	shared_ptr<PkbRelationshipTable> getUsesTable() {
		return this->relationshipTables[USES_TABLE];
	}
	shared_ptr<PkbRelationshipTable> getModifiesTable() {
		return this->relationshipTables[MODIFIES_TABLE];
	}

	/*
		Converts an SP entity to a pkb entity with the correct underlying behaviour.
		TODO: consider refactoring this. For now, we have a dependency on an external type from the SP. this was done
			for speed of development, and also the two-way coupling breaks abstraction in a minimal and arguably trivial way.
			Better practice would be to have an external contract dataclass, SP convert to that and pass into the PKB.
			Because it's currently not necessary, we don't do that.
	*/
	shared_ptr<PkbEntity> externalEntityToPkbEntity(Entity entity);

	/*
		Converts external relationship object to a PKB relationship. 
		TODO: consider refactoring this. For now, we have a dependency on an external type from the SP. this was done
			for speed of development, and also the two-way coupling breaks abstraction in a minimal and arguably trivial way. 
			Better practice would be to have an external contract dataclass, SP convert to that and pass into the PKB. 
			Because it's currently not necessary, we don't do that.
	*/
	shared_ptr<PkbRelationship> externalRelationshipToPkbRelationship(Relationship relationship);

	/*
		Maps the supported relationship types to an internal table.
	*/
	shared_ptr<PkbRelationshipTable> getTableByRelationshipType(PKBTrackedRelationshipType relationshipType);

	/*
		Converts an internal Pkb entity to a pql entity used in the qps.
	*/
	PQLEntity pkbEntityToQpsPqlEntity(shared_ptr<PkbEntity> entity);


	/*
		Converts an internal pkb pattern to a pql pattern used in the qps.
	*/
	PQLPattern pkbPatternToPqlPattern(shared_ptr<PkbPattern> pattern);

	/*
		Retrieves assign statements by lhs and rhs.
	*/
	vector<PQLPattern> retrieveAssignPatterns(ClauseArgument lhs, ClauseArgument rhs);

	/*
		Retrieves if patterns by lhs.
	*/
	vector<PQLPattern> retrieveIfPatterns(ClauseArgument lhs);

	/*
		Retrieves while patterns by lhs.
	*/
	vector<PQLPattern> retrieveWhilePatterns(ClauseArgument lhs);

	/*
		Helper function to check if retrieving the relationship, while semantically and syntacticall correct, is even 
		possible. e.g. Follows(s,s) is not possible
	*/
	bool canShortCircuitRetrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs);


	/*
		Helper function to convert a clause argument into a pkb entity it's trying to specify.
	*/
	shared_ptr<PkbEntity> convertClauseArgumentToPkbEntity(ClauseArgument clause);

	/* 
		Helper function to filter pkb statement entities by their type and convert them to PQLEntities.
	*/
	vector<PQLEntity> filterAndConvertStatementEntities(vector<shared_ptr<PkbEntity>> statements, PKBTrackedStatementType pkbTrackedStatementType);
public: 
	PKB() {}

	/*
		Add extracted relationships from the SP.
	*/
	void addRelationships(vector<Relationship> relationships) override;
	
	/*
		Add extracted patterns from the SP.
	*/
	void addPatterns(vector<Pattern> patterns) override;

	/*
		Add extracted entities from the SP.
	*/
	void addEntities(vector<Entity> entities) override;

	/*
		Retrieves all procedure entities by name.
	*/
	vector<PQLEntity> retrieveProcedureEntityByName(string procedureName) override;

	/*
		Retrieves all procedure entities.
	*/
	vector<PQLEntity> retrieveAllProcedureEntities() override;

	/*
		Retrieves all statement entities by line number of a specified type.
	*/
	vector<PQLEntity> retrieveStatementByLineNumberAndType(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) override;
	
	/*
		Retrieves all statement entities of a specified type.
	*/
	vector<PQLEntity> retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) override;
	
	/*
		Retrieves all statement entities, regardless of type.
	*/
	vector<PQLEntity> retrieveAllStatementEntities() override;
	
	/*
		Retrieves all variables. 
	*/
	vector<PQLEntity> retrieveAllVariables() override;

	/*
		Retrieves all constants.
	*/
	vector<PQLEntity> retrieveAllConstants() override;

	/*
		Retrieves all variables by a name.
	*/
	vector<PQLEntity> retrieveVariableByName(string name) override;

	/*
		Retrieves all constants by a value.
	*/
	vector<PQLEntity> retrieveConstantByValue(int value) override;

	/*
		Retrieves all relationships by a lhs, rhs for relationships of a specified type.
	*/
	vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) override;
	
	/*
		Retrieves all relationships of a specified type.
	*/
	vector<PQLRelationship> retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType) override;

	/*
        Retrieves statements by lhs and rhs for Assign Patterns
    */
	vector<PQLPattern> retrievePatterns(PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs) override;

	/*
		Casts the PKB to its query handler interface as a shared pointer.
	*/
	shared_ptr<PKBQueryHandler> getQueryHandler();

	/*
		Casts the PKB to its update handler interface as a shared pointer.
	*/
	shared_ptr<PKBUpdateHandler> getUpdateHandler();

	/*
	*	Checks if SP entity exists in PKB
	*/
	bool containsEntity(Entity entity);

	/*
	*	Checks if SP Relationship exists in PKB
	*/
	bool containsRelationship(Relationship relationship);

};

// helper methods
/*
	A filter applied to a pkb statement. This is used
	in filtering statements
*/
typedef bool (*PkbStatementEntityFilter)(PkbStatementEntity* statement);

/*
	A filter applied to a pkb entity. This is used
	in filtering relationships by lhs and rhs.
*/
typedef bool (*PkbEntityFilter)(shared_ptr<PkbEntity> entity, ClauseArgument arg);
/*
	Converts a clause argument to a filter that can be applied to a PkbEntity. This is used
	in filtering relationships by lhs and rhs. We provide a flag to return a dummy filter 
	that always evaluates to true for ease.
*/
PkbEntityFilter getFilterFromClauseArgument(ClauseArgument arg, bool alwaysTrue = false);
