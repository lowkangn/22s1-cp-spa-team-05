#pragma once

#include <qps/exceptions/PQLSyntaxError.h>

#include <stdexcept>
#include <string>
#include <unordered_map>

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
