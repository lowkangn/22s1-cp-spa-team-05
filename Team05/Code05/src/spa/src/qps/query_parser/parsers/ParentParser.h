#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>

class ParentParser : public SuchThatClauseParser {
private:
	bool isStar;
public:
	ParentParser(list<PQLToken>& tokens, unordered_map<string, ArgumentType> declarations) :
		SuchThatClauseParser(tokens, declarations) {
		this->isStar = false;
	};

	bool isCorrectClauseType(PQLToken clauseTypeToken) override;

	void checkArguments(list<ClauseArgument>& args) override;

	shared_ptr<RelationshipClause> createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) override;

};