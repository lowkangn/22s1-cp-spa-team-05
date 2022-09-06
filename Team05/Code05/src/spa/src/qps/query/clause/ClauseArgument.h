#pragma once

#include <string>
#include <qps/query_parser/ArgumentType.h>

using namespace std;

class ClauseArgument {
private:
    string identifier;
	ArgumentType type;
public:
	ClauseArgument(string identifier, ArgumentType type) {
		this->identifier = identifier;
		this->type = type;
	};

	bool isWildcard() {
		return this->type == ArgumentType::WILDCARD;
	}
	
	bool isStmtRefNoWildcard() {
		return this->type == ArgumentType::STMT
            || this->type == ArgumentType::READ
            || this->type == ArgumentType::PRINT
            || this->type == ArgumentType::ASSIGN
            || this->type == ArgumentType::CALL
            || this->type == ArgumentType::WHILE
            || this->type == ArgumentType::IF
			|| this->type == ArgumentType::LINE_NUMBER;
	}

	bool isEntRefNoWildcard() {
		return this->type == ArgumentType::PROCEDURE
            || this->type == ArgumentType::VARIABLE
            || this->type == ArgumentType::CONSTANT
			|| this->type == ArgumentType::STRING_LITERAL;
	}

	friend bool operator== (ClauseArgument first, ClauseArgument second);
};
