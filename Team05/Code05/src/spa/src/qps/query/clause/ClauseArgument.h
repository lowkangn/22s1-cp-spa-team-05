#pragma once

#include <string>
#include <qps/query_parser/ArgumentType.h>
#include <qps/exceptions/PQLError.h>

using namespace std;

class ClauseArgument {
private:

	string identifier;
	ArgumentType type;

	ClauseArgument(string identifier, ArgumentType type) {
		this->identifier = identifier;
		this->type = type;
	};

public:

	static ClauseArgument generateStmtArg(string name) {
		return ClauseArgument(name, ArgumentType::STMT);
	}

	bool isStmtSynonym() {
		return this->type == ArgumentType::STMT;
	}

	static ClauseArgument generateAssignArg(string name) {
		return ClauseArgument(name, ArgumentType::ASSIGN);
	}

	bool isAssignSynonym() {
		return this->type == ArgumentType::ASSIGN;
	}

	static ClauseArgument generatePrintArg(string name) {
		return ClauseArgument(name, ArgumentType::PRINT);
	}

	bool isPrintSynonym() {
		return this->type == ArgumentType::PRINT;
	}

	static ClauseArgument generateReadArg(string name) {
		return ClauseArgument(name, ArgumentType::READ);
	}

	bool isReadSynonym() {
		return this->type == ArgumentType::READ;
	}

	static ClauseArgument generateCallArg(string name) {
		return ClauseArgument(name, ArgumentType::CALL);
	}

	bool isCallSynonym() {
		return this->type == ArgumentType::CALL;
	}

	static ClauseArgument generateWhileArg(string name) {
		return ClauseArgument(name, ArgumentType::WHILE);
	}

	bool isWhileSynonym() {
		return this->type == ArgumentType::WHILE;
	}

	static ClauseArgument generateIfArg(string name) {
		return ClauseArgument(name, ArgumentType::IF);
	}

	bool isIfSynonym() {
		return this->type == ArgumentType::IF;
	}

	static ClauseArgument generateProcedureArg(string name) {
		return ClauseArgument(name, ArgumentType::PROCEDURE);
	}

	bool isProcedureSynonym() {
		return this->type == ArgumentType::PROCEDURE;
	}

	static ClauseArgument generateVariableArg(string name) {
		return ClauseArgument(name, ArgumentType::VARIABLE);
	}

	bool isVariableSynonym() {
		return this->type == ArgumentType::VARIABLE;
	}

	static ClauseArgument generateConstantArg(string name) {
		return ClauseArgument(name, ArgumentType::CONSTANT);
	}

	bool isConstantSynonym() {
		return this->type == ArgumentType::CONSTANT;
	}

	static ClauseArgument generateLineNumberArg(string lineNum) {
		return ClauseArgument(lineNum, ArgumentType::LINE_NUMBER);
	}

	bool isLineNumber() {
		return this->type == ArgumentType::LINE_NUMBER;
	}

	static ClauseArgument generateStringLiteralArg(string str) {
		return ClauseArgument(str, ArgumentType::STRING_LITERAL);
	}

	bool isStringLiteral() {
		return this->type == ArgumentType::STRING_LITERAL;
	}

	static ClauseArgument generateWildcardArg() {
		return ClauseArgument("_", ArgumentType::WILDCARD);
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
