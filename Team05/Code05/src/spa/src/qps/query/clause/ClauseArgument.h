#pragma once

#include <string>

using namespace std;

enum class ArgumentType {
	SYNONYM,
	LINE_NUMBER,
	STRING_LITERAL,
	WILDCARD
};

class ClauseArgument {
private:
	ArgumentType type;
	string identifier;
public:
	ClauseArgument(string identifier, ArgumentType type) {
		this->identifier = identifier;
		this->type = type;
	};
	friend bool operator== (ClauseArgument first, ClauseArgument second);
};


