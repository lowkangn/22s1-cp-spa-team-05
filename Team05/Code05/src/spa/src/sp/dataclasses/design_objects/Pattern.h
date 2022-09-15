#pragma once
#include <vector>
#include <string>

#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

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

	bool isAssignPattern() {
		return this->entity.isAssignEntity();
	}

	Entity getEntity() {
		return this->entity;
	}

	string getLhs() {
		return this->lhs;
	}

	string getRhs() {
		return this->rhs;
	}
};
