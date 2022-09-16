#pragma once

#include <string>
#include <qps/query_parser/ArgumentType.h>
#include <qps/exceptions/PQLError.h>

using namespace std;

const string WILDCARD_IDENTIFIER = "_";

class ClauseArgument {
private:
	string identifier;
	ArgumentType type;

	ClauseArgument(string identifier, ArgumentType type) {
		this->identifier = identifier;
		this->type = type;
	};
public:
	// Factory methods
	// TODO: Remove all uses of this factory method.
	static ClauseArgument createArgument(string identifier, ArgumentType type) {
		return ClauseArgument(identifier, type);
	}

	static ClauseArgument createAssignArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::ASSIGN);
	}

	static ClauseArgument createCallArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::CALL);
	}

	static ClauseArgument createConstantArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::CONSTANT);
	}

	static ClauseArgument createIfArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::IF);
	}

	static ClauseArgument createLineNumberArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::LINE_NUMBER);
	}

	static ClauseArgument createPrintArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::PRINT);
	}

	static ClauseArgument createProcedureArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::PROCEDURE);
	}

	static ClauseArgument createReadArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::READ);
	}

	static ClauseArgument createStmtArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::STMT);
	}

	static ClauseArgument createStringLiteralArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::STRING_LITERAL);
	}

	static ClauseArgument createVariableArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::VARIABLE);
	}

	static ClauseArgument createWhileArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::WHILE);
	}

	static ClauseArgument createWildcardArg() {
		return ClauseArgument(WILDCARD_IDENTIFIER, ArgumentType::WILDCARD);
	}

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
