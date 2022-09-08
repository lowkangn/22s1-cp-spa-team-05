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
enum class EntityType {PROGRAM, PROCEDURE, STMTLIST, STMT, VARIABLE, CONSTANT, LINENUMBER, UNDEFINED};

class EntityIdentifier {
private:
	Token token;
	string asString;
	// TODO: add validation at initialization
public:
	// Refer to https://stackoverflow.com/questions/68049611/no-default-constructor-exists-for-class-in-c for the following declaration usage
	EntityIdentifier(const Token &token, const string asString) : token(token), asString(asString) {
		this->token = token;
		this->asString = asString;
	}

    string getString() {
        return this->asString;
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
	Entity(EntityType type, int lineNumber, Token &token) : type(type), line(lineNumber), identifier(token, token.getString()) {
		this->type = type;
		this->line = lineNumber;
		EntityIdentifier identifier = EntityIdentifier{token, token.getString()};
		this->identifier = identifier;
	}
	 
	EntityType getType() {
		return this->type;
	}

    int getLine() {
        return this->line;
    }

    string toString() {
        return this->identifier.getString();
    }

	bool equals(Entity &other) {
		return this->type == other.getType() && this->line == other.line && this->identifier.equals(other.identifier);
	}
};

