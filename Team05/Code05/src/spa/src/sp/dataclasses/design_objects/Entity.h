#pragma once

#include <vector>
#include <string>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

/*
	Enumeration of possible entity types.
*/
enum class EntityType {PROCEDURE, STMTLIST, STMT, READ, PRINT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT};

class EntityIdentifier {
private:
	Token token;
	string asString;
	// TODO: add validation at initialization
};

/*
	Encapsulates an entity.
*/
class Entity {
private:
	EntityType type;
	int line;
	EntityIdentifier identifier;
};

