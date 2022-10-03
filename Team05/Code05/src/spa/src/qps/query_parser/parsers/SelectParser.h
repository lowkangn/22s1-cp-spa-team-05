#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SemanticChecker.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/SelectClause.h>

class SelectParser : public ClauseParser {
public:
	SelectParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
		ClauseParser(tokens, declarations) {};

	/* Parses this parser's tokens into a Clause. (Template method:
	   behaviour depends on implementation of virtual functions) */
	shared_ptr<SelectClause> parse();

	/* Returns true if the clauseTypeToken matches the clause that the Parser parses */
	virtual bool isCorrectClauseType(PQLToken clauseTypeToken) = 0;

	/* Extracts the arguments of the clause. Consumes all tokens in the clause after 'Select' */
	virtual list<ClauseArgument> extractArguments() = 0;

	/* Creates the clause given the clause type and arguments */
	virtual shared_ptr<SelectClause> createClause(list<ClauseArgument>& args) = 0;

};
