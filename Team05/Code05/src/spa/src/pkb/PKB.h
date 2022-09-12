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

#include <string>
#include <vector>
using namespace std;

class PKB : public PKBQueryHandler, public PKBUpdateHandler {
private: 
	// variables, statement and procedures
	PkbEntityTable variableTable;
	PkbEntityTable statementTable;
	PkbEntityTable proceduresTable;

	// relationships


	// patterns
	
public: 
	PKB() {
		// initialize entity tables

	}
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
