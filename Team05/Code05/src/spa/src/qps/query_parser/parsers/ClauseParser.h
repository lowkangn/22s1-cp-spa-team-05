#pragma once

#include <list>
#include <unordered_map>

#include <qps/exceptions/PQLError.h>
#include <qps/query_tokenizer/PQLToken.h> 
#include <qps/query_parser/DesignEntity.h>
#include <qps/query/clause/Clause.h>
#include <qps/query/clause/ClauseArgument.h>

using namespace std;

class ClauseParser {
protected:
	list<PQLToken> tokens;
	unordered_map<string, DesignEntity> declarations;

	ClauseArgument parseSynonym();

	ClauseArgument parseStringLiteral();

	ClauseArgument parseStatementNumber();

	ClauseArgument parseWildcard();

	void consumeClauseOpen();

	void consumeClauseMiddle();

	void consumeClauseClose();

public:

	ClauseParser(list<PQLToken> tokens, unordered_map<string, DesignEntity> declarations) {
		this->tokens = tokens;
		this->declarations = declarations;
	}

	list<PQLToken> getRemainingTokens();

	virtual shared_ptr<Clause> parse() = 0;
};