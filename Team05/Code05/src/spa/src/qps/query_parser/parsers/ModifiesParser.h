#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

class ModifiesParser : public SuchThatClauseParser {
public:
	ModifiesParser(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) :
		SuchThatClauseParser(tokens, declarations) {};

	bool isCorrectClauseType(PQLToken clauseTypeToken) override;

	void checkArguments(list<ClauseArgument>& args) override;

	shared_ptr<RelationshipClause> createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) override;

};
