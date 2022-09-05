#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/DesignEntity.h>
#include <qps/query_parser/PQLToken.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/SelectClause.h>

class SelectParser : public ClauseParser {
public:
	SelectParser(list<PQLToken> tokens, unordered_map<string, DesignEntity> declarations) : 
		ClauseParser(tokens, declarations) {};

	/* Parses all declarations in the query. */
	shared_ptr<Clause>  parse() override;
};