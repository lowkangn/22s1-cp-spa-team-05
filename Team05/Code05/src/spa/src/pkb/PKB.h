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

#include <string>
#include <vector>
using namespace std;

class PKB : public PKBQueryHandler, public PKBUpdateHandler {
public: 
	PKB() {
		// initialize tables
	}

	vector<int> addRelationships(vector<Relationship> relationships) override;
	void addPatterns(vector<Pattern> patterns) override;
	vector<int> addEntities(vector<Entity> entities) override;
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
