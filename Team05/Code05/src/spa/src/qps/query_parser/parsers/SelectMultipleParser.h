#pragma once

#include <qps/query_parser/parsers/SelectParser.h>

class SelectMultipleParser : public SelectParser {
public:
	SelectMultipleParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
		SelectParser(tokens, declarations) {};

	/* Extracts the arguments of the clause. Consumes all tokens in the clause after 'Select' */
	list<ClauseArgument> extractArguments() override;

	/* Creates the clause given the clause type and arguments */
	shared_ptr<SelectClause> createClause(list<ClauseArgument>& args) override;

	/* Returns true if the clauseTypeToken matches the clause that the Parser parses */
	bool isCorrectClauseType(PQLToken clauseTypeToken) override;

};
