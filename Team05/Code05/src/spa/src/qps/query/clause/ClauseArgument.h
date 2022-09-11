#pragma once

#include <string>
#include <qps/query_parser/ArgumentType.h>
#include <qps/exceptions/PQLError.h>

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

    bool isStmtSynonym() {
		return this->type == ArgumentType::STMT;
	}

    bool isAssignSynonym() {
		return this->type == ArgumentType::ASSIGN;
	}

    bool isPrintSynonym() {
		return this->type == ArgumentType::PRINT;
	}

    bool isReadSynonym() {
		return this->type == ArgumentType::READ;
	}

    bool isCallSynonym() {
		return this->type == ArgumentType::CALL;
	}

    bool isWhileSynonym() {
		return this->type == ArgumentType::WHILE;
	}

    bool isIfSynonym() {
		return this->type == ArgumentType::IF;
	}

    bool isProcedureSynonym() {
		return this->type == ArgumentType::PROCEDURE;
	}

    bool isVariableSynonym() {
		return this->type == ArgumentType::VARIABLE;
	}

    bool isConstantSynonym() {
		return this->type == ArgumentType::CONSTANT;
	}

    bool isLineNumber() {
		return this->type == ArgumentType::LINE_NUMBER;
	}

    bool isStringLiteral() {
		return this->type == ArgumentType::STRING_LITERAL;
	}

    bool isWildcard() {
        return this->type == ArgumentType::WILDCARD;
    }

	bool isSynonym() {
		return isStmtSynonym()
			|| isReadSynonym()
			|| isPrintSynonym()
			|| isAssignSynonym()
			|| isCallSynonym()
			|| isWhileSynonym()
			|| isIfSynonym()
			|| isProcedureSynonym()
			|| isVariableSynonym()
			|| isConstantSynonym();
	}

	bool isStmtRefNoWildcard() {
		return isStmtSynonym()
			|| isReadSynonym()
			|| isPrintSynonym()
			|| isAssignSynonym()
			|| isCallSynonym()
			|| isWhileSynonym()
			|| isIfSynonym()
			|| isLineNumber();
	}

	bool isEntRefNoWildcard() {
		return isProcedureSynonym()
			|| isVariableSynonym()
			|| isConstantSynonym()
			|| isStringLiteral();
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
