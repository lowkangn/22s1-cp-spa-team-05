#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

/*
	Enumeration of possible entity types.
*/
enum class EntityType {PROCEDURE, STMTLIST, STMT, VARIABLE, CONSTANT, LINENUMBER, UNDEFINED};

class EntityIdentifier {
private:
	Token token;
	string asString;
	// TODO: add validation at initialization
public:
	EntityIdentifier(const Token &token, const string asString) : token(token), asString(asString) {
		this->token = token;
		this->asString = asString;
	}

	bool equals(EntityIdentifier& other) {
		return this->token.equals(token) && this->asString == other.asString;
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
	Entity(EntityType type, int lineNumber, const Token &token, const string &asString) : type(type), line(lineNumber), identifier(token, asString) {
		this->type = type;
		this->line = lineNumber;
		EntityIdentifier identifier = EntityIdentifier{token, asString};
		this->identifier = identifier;
	}
	 
	EntityType getType() {
		return this->type;
	}

	bool equals(Entity &other) {
		return this->type == other.getType() && this->line == other.line && this->identifier.equals(other.identifier);
	}
};

