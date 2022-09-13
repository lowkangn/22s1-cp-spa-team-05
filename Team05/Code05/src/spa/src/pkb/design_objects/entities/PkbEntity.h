#pragma once
#include <string>

using namespace std;

enum class PkbEntityType {
	VARIABLE,
	STATEMENT,
	PROCEDURE
};

/*
	Parent class for all variables, statements, constants and procedures.
*/
class PkbEntity {
protected: 
	PkbEntityType entityType;
	PkbEntity(PkbEntityType entityType) {
		this->entityType = entityType;
	}
public: 

	bool isVariable() {
		return this->entityType == PkbEntityType::VARIABLE;
	}

	bool isStatement() {
		return this->entityType == PkbEntityType::STATEMENT;
	}

	bool isProcedure() {
		return this->entityType == PkbEntityType::PROCEDURE;
	}

	virtual string getKey() = 0;

	virtual string getIdentifier() = 0;

	virtual int getLineNumber() = 0;

	virtual bool equals(shared_ptr<PkbEntity> other) = 0;

};