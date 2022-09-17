#pragma once

#include <string>
#include <stdexcept>

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
    WILDCARD
};

ArgumentType getDesignEntityArgumentType(string argumentType);
