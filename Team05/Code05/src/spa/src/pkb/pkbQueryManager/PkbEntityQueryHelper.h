#pragma once


#include <assert.h>
#include <optional>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

using namespace std;

typedef bool (*PkbStatementEntityFilter)(PkbStatementEntity* statement);

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

	/*
		Creates a filter from tracked statement types.
	*/
	PkbStatementEntityFilter getFilterToCheckForStatementType(PkbStatementType t, bool defaultTrue = false) {
		PkbStatementEntityFilter filter = [](PkbStatementEntity* statement) {
			return true;
		};

		if (defaultTrue) {
			return filter;
		}

		// go through switch case to get specific filter by type
		if (t == PkbStatementType::READ) {
			filter = [](PkbStatementEntity* statement) {
				return statement->isReadStatement();
			};
		}
		else if (t == PkbStatementType::PRINT) {

			filter = [](PkbStatementEntity* statement) {
				return statement->isPrintStatement();
			};
		}
		else if (t == PkbStatementType::WHILE) {
			filter = [](PkbStatementEntity* statement) {
				return statement->isWhileStatement();
			};
		}
		else if (t == PkbStatementType::IF) {
			filter = [](PkbStatementEntity* statement) {
				return statement->isIfStatement();
			};
		}
		else if (t == PkbStatementType::ASSIGN) {
			filter = [](PkbStatementEntity* statement) {
				return statement->isAssignStatement();
			};
		}
		else if (t == PkbStatementType::CALL) {
			filter = [](PkbStatementEntity* statement) {
				return statement->isCallStatement();
			};
		}
		else {
			throw PkbException("Unknown statement type to filter for!");
		}

		return filter;
	}

};