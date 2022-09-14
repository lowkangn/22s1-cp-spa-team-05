#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>

class Pattern {
private: 
	Pattern(Entity entity, string lhs, string rhs) : entity(entity), lhs(lhs), rhs(rhs) {
		this->entity = entity;
		this->lhs = lhs;
		this->rhs = rhs;
	}
	Entity entity;
	string lhs;
	string rhs;
public:
	static Pattern createAssignPattern(int lineNumber, string lhs, string rhs) {
		Entity entity = Entity::createAssignEntity(lineNumber);
		return Pattern(entity, lhs, rhs);
	}
	static Pattern createIfPattern(int lineNumber, string lhs, string rhs) {
		Entity entity = Entity::createIfEntity(lineNumber);
		return Pattern(entity, lhs, rhs);
	}
	static Pattern createWhilePattern(int lineNumber, string lhs, string rhs) {
		Entity entity = Entity::createWhileEntity(lineNumber);
		return Pattern(entity, lhs, rhs);
	}

	bool equals(Pattern other) {
		return this->entity.equals(other.entity) && this->lhs == other.lhs && this->rhs == other.rhs;
	}
};