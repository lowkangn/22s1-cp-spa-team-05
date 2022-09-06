#pragma once

#include <string>

using namespace std;

enum class ArgumentType {
    PROCEDURE,
    STMT,
    READ,
    PRINT,
    ASSIGN,
    CALL,
    WHILE,
    IF,
    VARIABLE,
    CONSTANT,
    LINE_NUMBER,
    STRING_LITERAL,
    WILDCARD
};

ArgumentType getDesignEntityArgumentType(string argumentType);