#pragma once
#include <string>
#include <memory>

using namespace std;

enum class PkbEntityType {
	VARIABLE,
	STATEMENT,
	PROCEDURE,
	CONSTANT,
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

	bool isConstant() {
		return this->entityType == PkbEntityType::CONSTANT;
	}

	virtual string getKey() = 0;

	virtual string getIdentifier() = 0;

	virtual int getLineNumber() = 0;

	virtual int getValue() = 0;

	virtual bool equals(shared_ptr<PkbEntity> other) = 0;

};