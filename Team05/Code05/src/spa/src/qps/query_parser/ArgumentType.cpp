#include <unordered_map>
#include <qps/query_parser/ArgumentType.h>

const string STMT_KEYWORD = "stmt";
const string READ_KEYWORD = "read";
const string PRINT_KEYWORD = "print";
const string ASSIGN_KEYWORD = "assign";
const string CALL_KEYWORD = "call";
const string WHILE_KEYWORD = "while";
const string IF_KEYWORD = "if";
const string PROCEDURE_KEYWORD = "procedure";
const string VARIABLE_KEYWORD = "variable";
const string CONSTANT_KEYWORD = "constant";

unordered_map<string, ArgumentType> designEntityMap({
    {STMT_KEYWORD, ArgumentType::STMT},
    {READ_KEYWORD, ArgumentType::READ},
    {PRINT_KEYWORD, ArgumentType::PRINT},
    {ASSIGN_KEYWORD, ArgumentType::ASSIGN},
    {CALL_KEYWORD, ArgumentType::CALL},
    {WHILE_KEYWORD, ArgumentType::WHILE},
    {IF_KEYWORD, ArgumentType::IF},
    {PROCEDURE_KEYWORD, ArgumentType::PROCEDURE},
    {VARIABLE_KEYWORD, ArgumentType::VARIABLE},
    {CONSTANT_KEYWORD, ArgumentType::CONSTANT},
    }
);

ArgumentType getDesignEntityArgumentType(string argumentType) {
    try {
        return designEntityMap.at(argumentType);
    }
    catch (const out_of_range& oor) {
        throw PQLSyntaxError(argumentType + " is not supported as a Design Entity");
    }
}

