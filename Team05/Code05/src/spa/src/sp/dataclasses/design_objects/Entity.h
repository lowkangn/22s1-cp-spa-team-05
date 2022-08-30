#pragma once

#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

/*
	Enumeration of possible entity types.
*/
enum class EntityType {};

/*
	Encapsulates an entity.
*/
class Entity {
private:
	EntityType type;
	int line;
	EntityIdentifier identifier;

};

class EntityIdentifier {
private:
	Token token;
	string asString;
	// TODO: add validation at initialization
};