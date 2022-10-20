#pragma once

#include <qps/query_parser/query_parser_states/QueryParserState.h>
#include <qps/query_parser/parsers/PatternParser.h>

class PatternState : public QueryParserState {
public:
	PatternState(QueryParser* context) : QueryParserState(context) {}

	void parseOneClause(list<PQLToken>& tokens, unordered_map<string, ArgumentType>& declarations) override;
};