#pragma once

#include <qps/query_parser/query_parser_states/QueryParserState.h>
#include <qps/query_parser/parsers/WithParser.h>

class WithState : public QueryParserState {
public:
	WithState(QueryParser* context) : QueryParserState(context) {}

	void parseOneClause(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) override;
};