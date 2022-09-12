#pragma once
#include <pkb/design_objects/entities/PkbEntity.h>
#include <pkb/PkbException.h>
#include <string>
using namespace std;
/*
	This class encapsulates a procedure stored in the pkb. A procedure
	can be identified primarily by its string.
*/
class PkbProcedureEntity : public PkbEntity {
private:
	string identifier;
public:
	PkbProcedureEntity(string identifier) : PkbEntity (PkbEntityType::PROCEDURE) {
		this->identifier = identifier;
	}

	/*
		Boolean method to check if a given variable has this identifier.
	*/
	bool hasIdentifier(string identifierToMatch) {
		return this->identifier == identifierToMatch;
	}

	/*
		Overriding the equals operator.
	*/
	const bool operator==(const PkbProcedureEntity& other) const {
		return other.identifier == this->identifier;
	}

	/*
		Returns a unique key.
	*/
	string getKey() override {
		return this->identifier;
	}

	/*
		Gets the identifier.
	*/
	string getIdentifier() override {
		return this->identifier;
	}

	/*
		Gets the line number.
	*/
	int getLineNumber() override {
		throw PkbException("Procedure does not have a line number!");
	}

	bool equals(shared_ptr<PkbEntity> other) override {
		if (!other->isProcedure()) {
			return false;
		}
		return this->getIdentifier() == other->getIdentifier();
	}
};
