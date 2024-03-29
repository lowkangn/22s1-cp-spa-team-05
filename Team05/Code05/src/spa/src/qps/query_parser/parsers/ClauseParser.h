#pragma once

#include <assert.h>

#include <qps/exceptions/PQLLogicError.h>
#include <qps/exceptions/PQLSemanticError.h>
#include <qps/exceptions/PQLSyntaxError.h>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SemanticChecker.h>
#include <qps/query_tokenizer/PQLToken.h>

#include <list>
#include <stack>
#include <string>
#include <unordered_map>

using namespace std;

/* ClauseParser is an abstract class for the parsing of 
   all clauses. It contains default methods that are common
   in the parsing process across clauses.
*/
class ClauseParser : public SemanticChecker {
protected:
    list<PQLToken>& tokens;

    /* Boolean for checking that getRemainingTokens is not called before parse */
    bool isParseCompleted = false;

    /* A mapping from each synonym to its type */
    const unordered_map<string, ArgumentType>& declarations;

    /* ==== Methods to parse a single argument of the clause and consume the token(s) used. ==== */

    ClauseArgument parseOneStmtRefOrEntRef();

    ClauseArgument parseSynonym();

    ClauseArgument parseStringLiteral();

    ClauseArgument parseStatementNumber();

    ClauseArgument parseWildcard();

    /* ==== Methods ensure a delimiter is present and consume it ==== */

    /* Consumes the open bracket token. */
    void consumeOpenBracket();

    /* Consumes the comma token. */
    void consumeComma();

    /* Consumes the close bracket token. */
    void consumeCloseBracket();

public:
    ClauseParser(
        list<PQLToken>& tokens,
        const unordered_map<string, ArgumentType>& declarations) : tokens(tokens),
                                                                   declarations(declarations) {
        this->semanticErrorMessage = NO_SEMANTIC_ERROR_MESSAGE;
    }
};
