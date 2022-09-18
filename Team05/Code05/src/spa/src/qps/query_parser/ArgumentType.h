#pragma once

#include <string>
#include <stdexcept>

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
	PATTERN_STRING_WITH_WILDCARDS
};

ArgumentType getDesignEntityArgumentType(string argumentType);
