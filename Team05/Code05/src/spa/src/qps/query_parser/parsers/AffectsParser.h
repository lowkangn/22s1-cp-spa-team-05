#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>

class AffectsParser : public SuchThatClauseParser {
private:
	bool isStar;
public:
	AffectsParser(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) :
		SuchThatClauseParser(tokens, declarations) {
		this->isStar = false;
	};

	bool isCorrectClauseType(PQLToken clauseTypeToken) override;

	void checkArguments(list<ClauseArgument>& args) override;

	shared_ptr<RelationshipClause> createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) override;

};
