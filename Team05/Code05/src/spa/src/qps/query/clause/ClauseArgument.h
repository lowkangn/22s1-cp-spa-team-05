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
