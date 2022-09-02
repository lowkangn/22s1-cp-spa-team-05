#pragma once

#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

/*
	Enumeration of possible entity types.
*/
enum class EntityType {};

template<>
struct hash<Entity> {
	size_t operator()(const Entity& key) {
		return (hash<EntityType>()(key.type))
			^ (hash<EntityIdentifier>()(key.identifier) << 1);
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

};

template<>
struct hash<EntityIdentifier> {
	size_t operator()(const EntityIdentifier identifier) {
		return hash<string>()(identifier.asString);
	}
};

class EntityIdentifier {
private:
	Token token;
	string asString;
	// TODO: add validation at initialization
};