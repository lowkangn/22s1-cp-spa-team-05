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
	vector<PQLEntity> retrieveProcedureEntitiesByName(string procedureName) override;
	vector<PQLEntity> retrieveAllProcedureEntities() override;
	vector<PQLEntity> retrieveStatementEntitiesByLineNumber(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) override;
	vector<PQLEntity> retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) override;
	vector<PQLEntity> retrieveAllStatementEntities() override;
	vector<PQLEntity> retrieveAllConstants() override;
	vector<PQLEntity> retrieveAllVariables() override;
	vector<PQLEntity> retrieveVariablesByName(string name) override;
	vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) override;
	vector<PQLRelationship> retrieveRelationshipByType(PKBTrackedRelationshipType relationshipType) override;
	PKBQueryHandler getQueryHandler();
	PKBUpdateHandler getUpdateHandler();
};
