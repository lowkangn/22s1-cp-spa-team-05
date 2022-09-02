#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/Token.h>

enum class RelationshipType {
	MODIFIES,
	USES,
	FOLLOWS,
	FOLLOWST,
	PARENT,
	PARENTT
};

class Relationship {
private:
	Entity entity1;
	Entity entity2;
	RelationshipType type;
public:
	Relationship(Entity entity1, Entity entity2, RelationshipType type) {
		this->entity1 = entity1;
		this->entity2 = entity2;
		this->type = type;
	}

	bool equals(Relationship other) {
		return ((this->type == other.type) && (this->entity1 == other.entity1) && (this->entity2 == other.entity2));
	}
};