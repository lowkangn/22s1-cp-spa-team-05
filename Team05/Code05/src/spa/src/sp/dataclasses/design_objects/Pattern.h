#pragma once
#include <vector>
#include <string>

#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

class Pattern {
private: 
	Entity entity;
	string lhs;
	string rhs;

public: 
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
