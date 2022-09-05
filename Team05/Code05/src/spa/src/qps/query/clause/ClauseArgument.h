#pragma once

#include <string>

using namespace std;

enum class ArgumentType {
	STMTREF_SYNONYM,
	ENTREF_SYNONYM,
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

	bool isWildcard() {
		return this->type == ArgumentType::WILDCARD;
	}
	
	bool isStmtRefNoWildcard() {
		return this->type == ArgumentType::STMTREF_SYNONYM
			|| this->type == ArgumentType::LINE_NUMBER;
	}

	bool isEntRefNoWildcard() {
		return this->type == ArgumentType::ENTREF_SYNONYM
			|| this->type == ArgumentType::STRING_LITERAL;
	}

	friend bool operator== (ClauseArgument first, ClauseArgument second);
};


