#include <unordered_map>
#include "ArgumentType.h"
#include <qps/exceptions/PQLError.h>

unordered_map<string, ArgumentType> designEntityMap({
    {"procedure", ArgumentType::PROCEDURE},
    {"stmt", ArgumentType::STMT},
    {"read", ArgumentType::READ},
    {"print", ArgumentType::PRINT},
    {"assign", ArgumentType::ASSIGN},
    {"call", ArgumentType::CALL},
    {"while", ArgumentType::WHILE},
    {"if", ArgumentType::IF},
    {"variable", ArgumentType::VARIABLE},
    {"constant", ArgumentType::CONSTANT},
    }
);

ArgumentType getDesignEntityArgumentType(string argumentType) {
    try {
        return designEntityMap.at(argumentType);
    }
    catch (const out_of_range& oor) {
        throw PQLError(argumentType + " is not supported as a Design Entity");
    }
}

