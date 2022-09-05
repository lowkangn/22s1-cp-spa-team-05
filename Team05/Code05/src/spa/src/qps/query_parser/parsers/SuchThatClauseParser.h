#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/DesignEntity.h>
#include <qps/query_parser/PQLToken.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

class SuchThatClauseParser : public ClauseParser {
public:
	SuchThatClauseParser(list<PQLToken> tokens, unordered_map<string, DesignEntity> declarations) :
		ClauseParser(tokens, declarations) {};

	/* Extracts the LHS and RHS arguments of the clause */
	pair<ClauseArgument, ClauseArgument> extractArguments();

	/* Parses one argument into a ClauseArgument */
	ClauseArgument parseOneArgument();
};