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
	}
public:
    // Factory methods
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

	bool isStmtSynonym() const {
		return this->type == ArgumentType::STMT;
	}

	bool isAssignSynonym() const {
		return this->type == ArgumentType::ASSIGN;
	}

	bool isPrintSynonym() const {
		return this->type == ArgumentType::PRINT;
	}

	bool isReadSynonym() const {
		return this->type == ArgumentType::READ;
	}

	bool isCallSynonym() const {
		return this->type == ArgumentType::CALL;
	}

	bool isWhileSynonym() const {
		return this->type == ArgumentType::WHILE;
	}

	bool isIfSynonym() const {
		return this->type == ArgumentType::IF;
	}

	bool isProcedureSynonym() const {
		return this->type == ArgumentType::PROCEDURE;
	}

	bool isVariableSynonym() const {
		return this->type == ArgumentType::VARIABLE;
	}

	bool isConstantSynonym() const {
		return this->type == ArgumentType::CONSTANT;
	}

	bool isLineNumber() const {
		return this->type == ArgumentType::LINE_NUMBER;
	}

	bool isStringLiteral() const {
		return this->type == ArgumentType::STRING_LITERAL;
	}

	bool isPatternString() const {
		return this->type == ArgumentType::PATTERN_STRING;
	}

	bool isWildcard() const {
		return this->type == ArgumentType::WILDCARD;
	}

	bool isPatternStringWithWildcards() const {
		return this->type == ArgumentType::PATTERN_STRING_WITH_WILDCARDS;
	}

	bool isValueAttribute() const {
		return this->type == ArgumentType::VALUE;
	}

	bool isProcNameAttribute() const {
		return this->type == ArgumentType::PROCNAME;
	}

	bool isStmtNumAttribute() const {
		return this->type == ArgumentType::STMTNUM;
	}

	bool isVarNameAttribute() const {
		return this->type == ArgumentType::VARNAME;
	}

	bool isAttributeName() const {
		return isValueAttribute()
			|| isProcNameAttribute()
			|| isStmtNumAttribute()
			|| isVarNameAttribute();
	}

	bool isUndeclaredSynonym() const {
		return this->type == ArgumentType::UNDECLARED_SYNONYM;
	}

	bool isSynonym() const {
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

	bool isStmtRefNoWildcard() const {
		return isStmtSynonym()
			|| isReadSynonym()
			|| isPrintSynonym()
			|| isAssignSynonym()
			|| isCallSynonym()
			|| isWhileSynonym()
			|| isIfSynonym()
			|| isLineNumber();
	}

	bool isEntRefNoWildcard() const {
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
	bool isExactReference() const {
		return (this->isLineNumber() || this->isStringLiteral());	
	}

	bool isSameType(ClauseArgument other) const {
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
			return std::hash<string>()(to_string(arg.getLineNumber()));
		} else {
			return std::hash<string>()(arg.getIdentifier());
		}
	}
};
