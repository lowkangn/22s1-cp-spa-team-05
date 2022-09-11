#pragma once

#include <string>
#include "../../exceptions/PQLError.h"

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

	bool isLineNumber() {
		return this->type == ArgumentType::LINE_NUMBER;
	}


	bool isIdentifier() {
		return this->type == ArgumentType::STRING_LITERAL;
	}
	
	bool isStmtRefNoWildcard() {
		return this->type == ArgumentType::STMTREF_SYNONYM
			|| this->type == ArgumentType::LINE_NUMBER;
	}

	bool isEntRefNoWildcard() {
		return this->type == ArgumentType::ENTREF_SYNONYM
			|| this->type == ArgumentType::STRING_LITERAL;
	}

	int getLineNumber() {
		if (!this->isLineNumber()) {
			throw PQLError("Trying to get line number, but clause argument is not!");
		}
		return stoi(this->identifier);
	}

	string getIdentifier() {
		if (this->isLineNumber()) {
			throw PQLError("Trying to get identifier, but clause argument is a line number!");
		}
		return this->identifier;
	}
	
	friend bool operator== (ClauseArgument first, ClauseArgument second);
};
