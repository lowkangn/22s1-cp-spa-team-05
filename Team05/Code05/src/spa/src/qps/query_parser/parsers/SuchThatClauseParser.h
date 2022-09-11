#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

/* SuchThatClauseParser is an abstract class for the parsing of
   such that clauses. It implements the extraction of their arguments.
*/
class SuchThatClauseParser : public ClauseParser {
public:
	SuchThatClauseParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
		ClauseParser(tokens, declarations) {};

	/* Extracts the LHS and RHS arguments of the such that clause */
	list<ClauseArgument> extractArguments();
};
