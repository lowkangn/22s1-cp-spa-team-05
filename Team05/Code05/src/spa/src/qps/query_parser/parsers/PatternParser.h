#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/PatternClause.h>

/* PatternParser is an abstract class for the parsing of pattern
 * clauses. It implements the extraction of their arguments.
 */
class PatternParser : public ClauseParser {
public:
	PatternParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
			ClauseParser(tokens, declarations) {};

	/* Parses this parser's tokens into a Clause. (Template method:
	   behaviour depends on implementation of virtual functions) */
	shared_ptr<PatternClause> parse();

	/* Extracts the LHS and RHS arguments of the such that clause */
	vector<ClauseArgument> extractArguments();

	/* Returns true if the clauseTypeToken matches the clause that the Parser parses */
	virtual bool isCorrectClauseType(PQLToken clauseTypeToken) = 0;

	/* Checks that the arguments fulfill the constraints of the clause; throws a PQL error otherwise */
	virtual void checkArguments(vector<ClauseArgument>& args) = 0;

	/* Creates the clause given the clause type and arguments */
	virtual shared_ptr<PatternClause> createClause(PQLToken clauseTypeToken, vector<ClauseArgument>& args) = 0;
};