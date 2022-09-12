#pragma once

#include <pkb/interfaces/PKBQueryHandler.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include "../../qps/query/clause/ClauseArgument.h"
#include "../../qps/query/clause/ClauseResult.h"
#include "../../qps/query/clause/PQLEntity.h"
#include "../../qps/query/clause/PQLRelationship.h"

#include <pkb/table_managers/PkbEntityTable.h>
#include <pkb/table_managers/PkbRelationshipTable.h>

#include <map>
#include <string>
#include <vector>
using namespace std;

const map<PkbRelationshipType, string> pkbRelationshipTypeToTableKey{
	{PkbRelationshipType::FOLLOWS, "follows"},
	{PkbRelationshipType::FOLLOWSSTAR, "followsStar"},
	{PkbRelationshipType::PARENT, "parent"},
	{PkbRelationshipType::PARENTSTAR, "parentStar"},
	{PkbRelationshipType::USES, "uses"},
	{PkbRelationshipType::USESSTAR, "usesStar"},
	{PkbRelationshipType::MODIFIES, "modifies"}
};


class PKB : public PKBQueryHandler, public PKBUpdateHandler {
private: 
	// variables, statement and procedures
	PkbEntityTable variableTable;
	PkbEntityTable statementTable;
	PkbEntityTable proceduresTable;

	// relationships
	map<string, PkbRelationshipTable> relationshipTables{
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::FOLLOWS], PkbRelationshipTable()},
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::FOLLOWSSTAR], PkbRelationshipTable()},
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::PARENT], PkbRelationshipTable()},
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::PARENTSTAR], PkbRelationshipTable()},
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::USES], PkbRelationshipTable()},
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::USESSTAR], PkbRelationshipTable()},
		{pkbRelationshipTypeToTableKey[PkbRelationshipType::MODIFIES], PkbRelationshipTable()},
	};

	// patterns

	// getters
	PkbRelationshipTable getFollowsTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::FOLLOWS];
	}
	PkbRelationshipTable getFollowsStarTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::FOLLOWSSTAR];
	}
	PkbRelationshipTable getParentTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::PARENT];
	}
	PkbRelationshipTable getParentStarTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::PARENTSTAR];
	}
	PkbRelationshipTable getUsesTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::USES];
	}
	PkbRelationshipTable getUsesStarTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::USESSTAR];
	}
	PkbRelationshipTable getModifiesTable() {
		return this->relationshipTables[pkbRelationshipTypeToTableKey[PkbRelationshipType::MODIFIES];
	}

	/*
		Converts an SP entity to a pkb entity with the correct underlying behaviour.
	*/
	shared_ptr<PkbEntity> spEntityToPkbEntity(Entity entity);

	/*
		Maps the supported relationship types to an internal table.
	*/
	PkbRelationshipTable getTableByRelationshipType(PKBTrackedRelationshipType relationshipType);

	/*
		Converts an internal Pkb entity to a pql entity used in the qps.
	*/
	PQLEntity pkbEntityToQpsPqlEntity(shared_ptr<PkbEntity> entity);

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
	PQLEntity retrieveProcedureEntityByName(string procedureName) override;

	/*
		Retrieves all procedure entities.
	*/
	vector<PQLEntity> retrieveAllProcedureEntities() override;

	/*
		Retrieves all statement entities by line number of a specified type.
	*/
	PQLEntity retrieveStatementEntityByLineNumber(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) override;
	
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
		Retrieves all variables by a name.
	*/
	PQLEntity retrieveVariableByName(string name) override;

	/*
		Retrieves all relationships by a lhs, rhs for relationships of a specified type.
	*/
	vector<PQLRelationship> retrieveRelationshipsByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) override;
	
	/*
		Retrieves all relationships of a specified type.
	*/
	PQLRelationship retrieveRelationshipByType(PKBTrackedRelationshipType relationshipType) override;
	
	/*
		Casts the PKB to its query handler interface as a shared pointer.
	*/
	shared_ptr<PKBQueryHandler> getQueryHandler();

	/*
		Casts the PKB to its update handler interface as a shared pointer.
	*/
	shared_ptr<PKBUpdateHandler> getUpdateHandler();
};

// helper methods
/*
	A filter applied to a pkb entity. This is used
	in filtering relationships by lhs and rhs.
*/
typedef bool (*PkbEntityFilter)(shared_ptr<PkbEntity> side);
/*
	Converts a clause argument to a filter that can be applied to a PkbEntity. This is used
	in filtering relationships by lhs and rhs.
*/
PkbEntityFilter getFilterFromClauseArgument(ClauseArgument arg);