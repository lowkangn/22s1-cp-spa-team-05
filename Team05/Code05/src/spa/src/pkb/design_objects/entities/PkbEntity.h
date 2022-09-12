#pragma once
#include <string>

using namespace std;

enum class PkbEntityType {
	VARIABLE,
	STATEMENT,
	PROCEDURE
};

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
};