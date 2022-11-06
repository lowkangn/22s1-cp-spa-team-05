#pragma once

#include <qps/query_parser/parsers/AffectsParser.h>
#include <qps/query_parser/parsers/CallsParser.h>
#include <qps/query_parser/parsers/FollowsParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/NextParser.h>
#include <qps/query_parser/parsers/ParentParser.h>
#include <qps/query_parser/parsers/UsesParser.h>
#include <qps/query_parser/query_parser_states/QueryParserState.h>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class SuchThatState : public QueryParserState {
public:
    explicit SuchThatState(QueryParser* context) : QueryParserState(context) {
    }

    void parseOneClause(
        list<PQLToken>& tokens,
        const unordered_map<string, ArgumentType>& declarations) override;
};
