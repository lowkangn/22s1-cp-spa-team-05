#pragma once

#include <qps/query_parser/query_parser_states/QueryParserState.h>
#include <qps/query_parser/parsers/CallsParser.h>
#include <qps/query_parser/parsers/FollowsParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/ParentParser.h>
#include <qps/query_parser/parsers/UsesParser.h>
#include <qps/query_parser/parsers/NextParser.h>

class SuchThatState : public QueryParserState {
public:
	SuchThatState(QueryParser* context) : QueryParserState(context) {}

	void parseOneClause(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) override;
};