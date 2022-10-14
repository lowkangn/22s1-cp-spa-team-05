#pragma once

#include <string>
#include <qps/query_parser/ArgumentType.h>
#include <qps/exceptions/PQLLogicError.h>

using namespace std;

const string WILDCARD_IDENTIFIER = "_";
const string EMPTY_IDENTIFIER = "";
const string VALUE_IDENTIFIER = "VALUE";
const string PROCNAME_IDENTIFIER = "PROCNAME";
const string VARNAME_IDENTIFIER = "VARNAME";
const string STMTNUM_IDENTIFIER = "STMTNUM";

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

	static ClauseArgument createPatternStringArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::PATTERN_STRING);
	}

	static ClauseArgument createWildcardArg() {
		return ClauseArgument(WILDCARD_IDENTIFIER, ArgumentType::WILDCARD);
	}

	static ClauseArgument createPatternStringWithWildcardsArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::PATTERN_STRING_WITH_WILDCARDS);
	}

	static ClauseArgument createValueAttributeArg(ClauseArgument synonym) {
		string identifier = synonym.getIdentifier() + VALUE_IDENTIFIER;
		return ClauseArgument(identifier, ArgumentType::VALUE);
	}

	static ClauseArgument createProcNameAttributeArg(ClauseArgument synonym) {
		string identifier = synonym.getIdentifier() + PROCNAME_IDENTIFIER;
		return ClauseArgument(identifier, ArgumentType::PROCNAME);
	}

	static ClauseArgument createStmtNumAttributeArg(ClauseArgument synonym) {
		string identifier = synonym.getIdentifier() + STMTNUM_IDENTIFIER;
		return ClauseArgument(identifier, ArgumentType::STMTNUM);
	}

	static ClauseArgument createVarNameAttributeArg(ClauseArgument synonym) {
		string identifier = synonym.getIdentifier() + VARNAME_IDENTIFIER;
		return ClauseArgument(identifier, ArgumentType::VARNAME);
	}

	static ClauseArgument createUndeclaredSynonymArg(string identifier) {
		return ClauseArgument(identifier, ArgumentType::UNDECLARED_SYNONYM);
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

	bool isPatternString() {
		return this->type == ArgumentType::PATTERN_STRING;
	}

	bool isWildcard() {
		return this->type == ArgumentType::WILDCARD;
	}

	bool isPatternStringWithWildcards() {
		return this->type == ArgumentType::PATTERN_STRING_WITH_WILDCARDS;
	}

	bool isValueAttribute() {
		return this->type == ArgumentType::VALUE;
	}

	bool isProcNameAttribute() {
		return this->type == ArgumentType::PROCNAME;
	}

	bool isStmtNumAttribute() {
		return this->type == ArgumentType::STMTNUM;
	}

	bool isVarNameAttribute() {
		return this->type == ArgumentType::VARNAME;
	}

	bool isAttributeName() {
		return isValueAttribute()
			|| isProcNameAttribute()
			|| isStmtNumAttribute()
			|| isVarNameAttribute();
	}

	bool isUndeclaredSynonym() {
		return this->type == ArgumentType::UNDECLARED_SYNONYM;
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
			|| isConstantSynonym()
			|| isUndeclaredSynonym();
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
			throw PQLLogicError("Trying to get line number, but clause argument is not!");
		}
		return stoi(this->identifier);
	}

	string getIdentifier() {
		if (this->isLineNumber()) {
			throw PQLLogicError("Trying to get identifier, but clause argument is a line number!");
		}
		return this->identifier;
	}

	/*
		Indicates if the clause argument is exact (e.g. "x", 1)
	*/
	bool isExactReference() {
		return (this->isLineNumber() || this->isStringLiteral());	
	}

	bool isSameType(ClauseArgument other) {
		return this->type == other.type;
	}

	friend bool operator== (ClauseArgument first, ClauseArgument second);

	friend bool operator!= (ClauseArgument first, ClauseArgument second);

	friend bool operator< (ClauseArgument first, ClauseArgument second);

};

template<> struct hash<ClauseArgument> {
	size_t operator()(ClauseArgument arg) const noexcept {
		// Can just use identifier since two different arguments won't have the same identifier
		if (arg.isLineNumber()) {
			return hash<string>{}(to_string(arg.getLineNumber()));
		} else {
			return hash<string>{}(arg.getIdentifier());
		}
	}
};
