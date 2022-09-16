#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/parsers/PatternParser.h>

class PatternAssignParser : public PatternParser {
public:
	PatternAssignParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
			PatternParser(tokens, declarations) {};

	bool isCorrectClauseType(PQLToken clauseTypeToken) override;

	void checkArguments(vector<ClauseArgument>& args) override;

	shared_ptr<PatternClause> createClause(PQLToken clauseTypeToken, vector<ClauseArgument>& args) override;

};
