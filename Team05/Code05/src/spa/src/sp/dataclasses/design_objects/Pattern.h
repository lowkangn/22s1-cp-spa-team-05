#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/Token.h>

class Pattern {
private: 
	Entity entity;
	vector<Token> lhs;
	vector<Token> rhs;
};