#pragma once

#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query_tokenizer/PQLToken.h>

#include <list>
#include <string>
#include <unordered_map>

/* Forward declaration to prevent cyclic inclusion issue :
see https://stackoverflow.com/questions/4685220/c-cyclic-inclusion-issue */
class QueryParser;

/* QueryParserState encapsulates a state during 
   the parsing of a constraint clause */
class QueryParserState {
protected:
    /* The QueryParser to which this state applies */
    QueryParser* context;

    explicit QueryParserState(QueryParser* context) : context(context) {
    }

public:
    /* Parses one clause by consuming PQLTokens for one clause from tokens  */
    virtual void parseOneClause(
        list<PQLToken>& tokens,
        const unordered_map<string, ArgumentType>& declarations) = 0;
};
