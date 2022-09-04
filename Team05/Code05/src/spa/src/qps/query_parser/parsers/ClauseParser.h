#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/PQLError.h>
#include <qps/query_parser/PQLToken.h> 
#include <qps/query_parser/DesignEntity.h>
#include <qps/query/clause/Clause.h>
#include <qps/query/clause/ClauseArgument.h>

using namespace std;

class ClauseParser {
protected:
	list<PQLToken> tokens;
	unordered_map<string, DesignEntity> declarations;
public:

	ClauseParser(list<PQLToken> tokens, unordered_map<string, DesignEntity> declarations) {
		this->tokens = tokens;
		this->declarations = declarations;
	}

	list<PQLToken> getRemainingTokens();

	ClauseArgument parseSynonym(PQLToken synonymToken);

	ClauseArgument parseStringLiteral(PQLToken strLiteralToken);

	ClauseArgument parseStatementNumber(PQLToken stmtNumToken);

	virtual Clause* parse() = 0;

};