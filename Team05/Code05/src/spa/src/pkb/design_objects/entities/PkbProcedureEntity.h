#pragma once
#include <pkb/design_objects/entities/PkbEntity.h>
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
	public bool hasIdentifier(string identifierToMatch) {
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
};

template <>
struct hash<PkbProcedureEntity>
{
	size_t operator()(const PkbProcedureEntity& k) const
	{
		// Compute individual hash values for two data members and combine them using XOR and bit shifting
		return hash<string>(k.getKey());
	}
};