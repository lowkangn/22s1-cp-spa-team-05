#pragma once

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLRelationship.h>
#include <qps/query/clause/PQLPattern.h>

#include <assert.h>

#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

#include <pkb/interfaces/PKBQueryHandler.h>

using namespace std;

class PkbQueryManager {
private: 
	// ==================== helper functions ====================

	// ******************** conversion ********************
	/*
		Converts an internal Pkb entity to a pql entity used in the qps.
	*/
	PQLEntity pkbEntityToQpsPqlEntity(shared_ptr<PkbEntity> entity);

	/*
		Converts an internal pkb pattern to a pql pattern used in the qps.
	*/
	PQLPattern pkbPatternToPqlPattern(shared_ptr<PkbPattern> pattern);


public: 
	// ******************** retrieval ********************
	// -------------------- all --------------------
	/*
		Retrieves all procedure entities.
	*/
	vector<PQLEntity> retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all statement entities, regardless of type.
	*/
	vector<PQLEntity> retrieveAllStatementEntities(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all variables.
	*/
	vector<PQLEntity> retrieveAllVariables(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all constants.
	*/
	vector<PQLEntity> retrieveAllConstants(shared_ptr<PkbRepository> repository);

	// -------------------- by type --------------------
	/*
		Retrieves all statement entities of a specified type.
	*/
	vector<PQLEntity> retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType, shared_ptr<PkbRepository> repository);

	/*
		Retrieves all relationships of a specified type.
	*/
	vector<PQLRelationship> retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType, shared_ptr<PkbRepository> repository);


	// -------------------- by identifier --------------------
	/*
		Retrieves all statement entities by line number of a specified type.
	*/
	optional<PQLEntity> retrieveStatementByLineNumberAndType(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType, shared_ptr<PkbRepository> repository);


	/*
		Retrieves all procedure entities by name.
	*/
	optional<PQLEntity> retrieveProcedureEntityByName(string procedureName, shared_ptr<PkbRepository> repository);


	/*
		Retrieves all variables by a name.
	*/
	optional<PQLEntity> retrieveVariableByName(string name, shared_ptr<PkbRepository> repository);

	/*
		Retrieves all constants by a value.
	*/
	optional<PQLEntity> retrieveConstantByValue(int value, shared_ptr<PkbRepository> repository);

	// -------------------- specifics --------------------
};