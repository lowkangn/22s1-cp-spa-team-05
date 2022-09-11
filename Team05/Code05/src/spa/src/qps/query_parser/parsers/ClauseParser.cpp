#include <qps/query_parser/parsers/ClauseParser.h>

shared_ptr<Clause> ClauseParser::parse() {
	PQLToken clauseTypeToken = this->tokens.front();
	assert(isCorrectClauseType(clauseTypeToken));
	this->tokens.pop_front();

	list<ClauseArgument> args = extractArguments();
	this->checkArguments(args);
	return createClause(clauseTypeToken, args);
}

ClauseArgument ClauseParser::parseSynonym() {
    PQLToken synonymToken = this->tokens.front();
    if (declarations.count(synonymToken.getTokenString()) == 0) {
        throw PQLError("Synonym not declared: " + synonymToken.getTokenString());
    }
    this->tokens.pop_front();
    return ClauseArgument(synonymToken.getTokenString(), declarations.at(synonymToken.getTokenString()));
}

ClauseArgument ClauseParser::parseOneArgument() {
	PQLToken token = this->tokens.front();
	if (token.isName()) {
		return parseSynonym();
	}
	else if (token.isQuote()) {
		return parseStringLiteral();
	}
	else if (token.isInteger()) {
		return parseStatementNumber();
	}
	else if (token.isUnderscore()) {
		return parseWildcard();
	}
	else {
		throw PQLError("Expected stmtRef or entRef, got: " + token.getTokenString());
	}
}

ClauseArgument ClauseParser::parseStringLiteral() {
	// Ignore '"' token
	this->tokens.pop_front();

	PQLToken token = this->tokens.front();
	if (!token.isName()) {
		throw PQLError("Expected name in quotes, got: " + token.getTokenString());
	}
	this->tokens.pop_front();
	if (this->tokens.empty() || !this->tokens.front().isQuote()) {
		throw PQLError("Expected closing quote");
	}
	this->tokens.pop_front();
	return ClauseArgument(token.getTokenString(), ArgumentType::STRING_LITERAL);
}

ClauseArgument ClauseParser::parseStatementNumber() {
	PQLToken stmtNumToken = this->tokens.front();
	this->tokens.pop_front();
	return ClauseArgument(stmtNumToken.getTokenString(), ArgumentType::LINE_NUMBER);
}

ClauseArgument ClauseParser::parseWildcard() {
	PQLToken wildCardToken = this->tokens.front();
	this->tokens.pop_front();
	return ClauseArgument(wildCardToken.getTokenString(), ArgumentType::WILDCARD);
}

void ClauseParser::consumeOpenBracket() {
	if (this->tokens.empty() || !this->tokens.front().isOpenBracket()) {
		throw PQLError("Expected Open bracket");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLError("Expected argument after open bracket");
	}
}

void ClauseParser::consumeComma() {
	if (this->tokens.empty() || !this->tokens.front().isComma()) {
		throw PQLError("Expected Comma");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLError("Expected argument after comma");
	}
}

void ClauseParser::consumeCloseBracket() {
	if (this->tokens.empty() || !this->tokens.front().isCloseBracket()) {
		throw PQLError("Expected Close bracket");
	}
	this->tokens.pop_front();
}

list<PQLToken> ClauseParser::getRemainingTokens() {
	return this->tokens;
}
