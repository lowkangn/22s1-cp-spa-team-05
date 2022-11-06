#pragma once

#include <qps/query_parser/parsers/WithParser.h>
#include <qps/query_parser/query_parser_states/QueryParserState.h>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class WithState : public QueryParserState {
public:
    explicit WithState(QueryParser* context) : QueryParserState(context) {
    }

    void parseOneClause(
        list<PQLToken>& tokens,
        const unordered_map<string, ArgumentType>& declarations) override;
};
