#pragma once

#include <vector>
#include <string>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

/*
	Enumeration of possible entity types.
*/
enum class EntityType {PROCEDURE, STMTLIST, STMT, VARIABLE, CONSTANT, LINENUMBER};

class EntityIdentifier {
private:
	Token token;
	string asString;
	// TODO: add validation at initialization
public:
	EntityIdentifier(Token token, string asString) {
		this->token = token;
		this->asString = asString;
	}
};

/*
	Encapsulates an entity.
*/
class Entity {
private:
	EntityType type;
	int line;
	EntityIdentifier identifier;
public:
	Entity(EntityType type, int lineNumber, Token token, string asString) {
		this->type = type;
		this->line = lineNumber;
		EntityIdentifier* identifier = new EntityIdentifier(token, asString);
		this->identifier = *identifier;
	}
};

