#pragma once


#include <assert.h>
#include <optional>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

using namespace std;

class PkbEntityQueryHelper {

public:

	/*
		Retrieves all procedure entities.
	*/
	vector<shared_ptr<PkbEntity>> retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all statement entities, regardless of type.
	*/
	vector<shared_ptr<PkbEntity>> retrieveAllStatementEntities(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all variables.
	*/
	vector<shared_ptr<PkbEntity>> retrieveAllVariables(shared_ptr<PkbRepository> repository);

	/*
		Retrieves all constants.
	*/
	vector<shared_ptr<PkbEntity>> retrieveAllConstants(shared_ptr<PkbRepository> repository);

	/*
		Given a key and a type specification, retrieves the object if it exists
		from the repository.
	*/
	shared_ptr<PkbEntity> retrieveEntityByTypeAndKey(string key, PkbEntityType type, shared_ptr<PkbRepository> repository);




};