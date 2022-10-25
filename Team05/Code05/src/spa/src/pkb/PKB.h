#pragma once

#include <pkb/interfaces/PKBQueryHandler.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLRelationship.h>

#include <pkb/pkbRepository/table_managers/PkbEntityTable.h>
#include <pkb/pkbRepository/table_managers/PkbRelationshipTable.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/graph_managers/PkbGraphManager.h>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbUpdateManager/PkbUpdateManager.h>
#include <pkb/pkbQueryManager/PkbQueryManager.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>

using namespace std;



class PKB : public PKBQueryHandler, public PKBUpdateHandler {
private: 
	
	// ======================== attributes ==============================
	shared_ptr<PkbRepository> repository = shared_ptr<PkbRepository>(new PkbRepository());
	PkbUpdateManager updateManager;
	PkbQueryManager queryManager;

	

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
		Retrieves all relationships by a lhs, rhs for relationships of a specified type, from tables.
	*/
	vector<PQLRelationship> retrieveRelationshipsFromTablesByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs);

	/*
		Retrieves all relationships by a lhs, rhs for relationships of a specified type, from graphs.
	*/
	vector<PQLRelationship> retrieveRelationshipsFromGraphsByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs);

	/* 
		Helper function to filter pkb statement entities by their type and convert them to PQLEntities.
	*/
	vector<PQLEntity> filterAndConvertStatementEntities(vector<shared_ptr<PkbEntity>> statements, PKBTrackedStatementType pkbTrackedStatementType);

public: 
	PKB() {}

	// ============================== Update handler API ==============================

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
		Add the control flow graph from the SP.
	*/
	void addCfg(shared_ptr<CFGNode> rootNode) override;

	// ============================== Retrieve handler API ==============================

	// ******************** ALL ********************

	/*
		Retrieves all procedure entities.
	*/
	vector<PQLEntity> retrieveAllProcedureEntities() override;

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

	// ******************** BY TYPE ********************

	/*
		Retrieves all statement entities of a specified type.
	*/
	vector<PQLEntity> retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) override;

	/*
		Retrieves all relationships of a specified type.
	*/
	vector<PQLRelationship> retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType) override;

	// ******************** BY IDENTIFIER ********************

	/*
		Retrieves all statement entities by line number of a specified type.
	*/
	optional<PQLEntity> retrieveStatementByLineNumberAndType(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) override;


	/*
		Retrieves all procedure entities by name.
	*/
	optional<PQLEntity> retrieveProcedureEntityByName(string procedureName) override;


	/*
		Retrieves all variables by a name.
	*/
	optional<PQLEntity> retrieveVariableByName(string name) override;

	/*
		Retrieves all constants by a value.
	*/
	optional<PQLEntity> retrieveConstantByValue(int value) override;

	// ******************** BY DETAIL ********************

	/*
		Retrieves all relationships by a lhs, rhs for relationships of a specified type.
	*/
	vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) override;
	
	
	/*
        Retrieves statements by lhs and rhs for Assign Patterns
    */
	vector<PQLPattern> retrievePatterns(PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs) override;

	
	// ******************** CONTAINS ********************

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
