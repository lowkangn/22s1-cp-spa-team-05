#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/SelectClause.h>

class SelectParser : public ClauseParser {
public:
	SelectParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
		ClauseParser(tokens, declarations) {};

	bool isCorrectClauseType(PQLToken clauseTypeToken) override;

	list<ClauseArgument> extractArguments() override;

	void checkArguments(list<ClauseArgument>& args) override;

	shared_ptr<Clause> createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) override;

};
