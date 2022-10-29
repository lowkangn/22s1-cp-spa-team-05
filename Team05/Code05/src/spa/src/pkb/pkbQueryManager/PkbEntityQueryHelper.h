#pragma once


#include <assert.h>
#include <optional>
#include <memory>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

using namespace std;

/*
	Statement entity filter type. Note that the return type is a raw pointer because of 
	how typdefs work in C++11. 

	See https://stackoverflow.com/questions/26151/template-typedefs-whats-your-work-around
*/
typedef bool (*PkbStatementEntityFilter)(shared_ptr<PkbStatementEntity> statement);

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
	PkbStatementEntityFilter getFilterToCheckForStatementType(PkbStatementType statementType, bool defaultTrue = false) {
		PkbStatementEntityFilter filter = [](shared_ptr<PkbStatementEntity> statement) {
			return true;
		};

		if (defaultTrue) {
			return filter;
		}

		// go through switch case to get specific filter by type
		if (statementType == PkbStatementType::READ) {
			filter = [](shared_ptr<PkbStatementEntity> statement) {
				return statement->isReadStatement();
			};
		}
		else if (statementType == PkbStatementType::PRINT) {

			filter = [](shared_ptr<PkbStatementEntity> statement) {
				return statement->isPrintStatement();
			};
		}
		else if (statementType == PkbStatementType::WHILE) {
			filter = [](shared_ptr<PkbStatementEntity> statement) {
				return statement->isWhileStatement();
			};
		}
		else if (statementType == PkbStatementType::IF) {
			filter = [](shared_ptr<PkbStatementEntity> statement) {
				return statement->isIfStatement();
			};
		}
		else if (statementType == PkbStatementType::ASSIGN) {
			filter = [](shared_ptr<PkbStatementEntity> statement) {
				return statement->isAssignStatement();
			};
		}
		else if (statementType == PkbStatementType::CALL) {
			filter = [](shared_ptr<PkbStatementEntity> statement) {
				return statement->isCallStatement();
			};
		}
		else {
			throw PkbException("Unknown statement type to filter for!");
		}

		return filter;
	}

};