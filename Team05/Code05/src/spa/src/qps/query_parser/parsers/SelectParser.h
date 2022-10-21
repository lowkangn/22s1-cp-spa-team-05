#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/AttributeClauseParser.h>
#include <qps/query/clause/SelectClause.h>

class SelectParser : public AttributeClauseParser {
protected:
	/* Returns true if the clauseTypeToken matches the clause that the Parser parses */
	virtual bool isCorrectClauseType(PQLToken clauseTypeToken) = 0;

	/* Extracts the arguments of the clause. Consumes all tokens in the clause after 'Select' */
	virtual list<ClauseArgument> extractArguments() = 0;

	/* Creates the clause given the clause type and arguments */
	virtual shared_ptr<SelectClause> createClause(list<ClauseArgument>& args) = 0;

public:
	SelectParser(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) :
		AttributeClauseParser(tokens, declarations) {};

	/* Parses this parser's tokens into a Clause. (Template method:
	   behaviour depends on implementation of virtual functions) */
	shared_ptr<SelectClause> parse();
};
