#pragma once
#include <pkb/design_objects/entities/PkbEntity.h>
#include <string>
using namespace std;
/*
	This class encapsulates a variable stored in the pkb. A variable 
	can be identified primarily by its string. 
*/
class PkbVariableEntity : public PkbEntity {
private: 
	string identifier;
public:
	PkbVariableEntity(string identifier) : PkbEntity(PkbEntityType::VARIABLE) {
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
	const bool operator==(const PkbVariableEntity& other) const {
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
struct hash<PkbVariableEntity>
{
	size_t operator()(const PkbVariableEntity& k) const
	{
		// Compute individual hash values for two data members and combine them using XOR and bit shifting
		return hash<string>(k.getKey());
	}
};