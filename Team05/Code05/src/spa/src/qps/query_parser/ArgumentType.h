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
    WILDCARD
};

ArgumentType getDesignEntityArgumentType(string argumentType);
