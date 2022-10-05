#pragma once

#include <string>
#include <stdexcept>
#include <unordered_map>

#include <qps/exceptions/PQLSyntaxError.h>

using namespace std;

enum class ArgumentType {
    STMT,
    READ,
    PRINT,
    ASSIGN,
    CALL,
    WHILE,
    IF,
    PROCEDURE,
    VARIABLE,
    CONSTANT,
    LINE_NUMBER,
    STRING_LITERAL,
	PATTERN_STRING,
    WILDCARD,
	PATTERN_STRING_WITH_WILDCARDS,
    PROCNAME,
    VARNAME,
    STMTNUM,
    VALUE,
    UNDECLARED_SYNONYM
};

ArgumentType getDesignEntityArgumentType(string argumentType);
