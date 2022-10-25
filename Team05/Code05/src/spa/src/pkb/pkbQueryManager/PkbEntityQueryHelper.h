#pragma once


#include <assert.h>
#include <optional>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

using namespace std;

class PkbEntityQueryHelper {

public:
	// ******************** retrieval ********************
	// -------------------- all --------------------
	/*
		Retrieves all procedure entities.
	*/
	vector<PkbEntity> retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all statement entities, regardless of type.
	*/
	vector<PkbEntity> retrieveAllStatementEntities(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all variables.
	*/
	vector<PkbEntity> retrieveAllVariables(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all constants.
	*/
	vector<PkbEntity> retrieveAllConstants(shared_ptr<PkbRepository> repository);

	/*
		Given a key and a type specification, retrieves the object if it exists
		from the repository.
	*/
	vector<PkbEntity> retrieveEntityByTypeAndKey(string key, PkbEntityType type, shared_ptr<PkbRepository> repository);




};