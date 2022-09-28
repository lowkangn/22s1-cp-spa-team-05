#include <qps/query_parser/parsers/ClauseParser.h>

ClauseArgument ClauseParser::parseSynonym() {
    PQLToken synonymToken = this->tokens.front();
	this->tokens.pop_front();
    if (declarations.count(synonymToken.getTokenString()) == 0) {
		this->semanticErrorMessage = "Synonym not declared: " + synonymToken.getTokenString();
		// we return a dummy ClauseArgument since there is a semanticError
		return ClauseArgument::createWildcardArg();
    }
    return ClauseArgument::createArgument(synonymToken.getTokenString(), declarations.at(synonymToken.getTokenString()));
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
		throw PQLSyntaxError("Expected stmtRef or entRef, got: " + token.getTokenString());
	}
}

ClauseArgument ClauseParser::parseStringLiteral() {
	// Ignore '"' token
	this->tokens.pop_front();

	PQLToken token = this->tokens.front();
	if (!token.isName()) {
		throw PQLSyntaxError("Expected name in quotes, got: " + token.getTokenString());
	}
	this->tokens.pop_front();

	if (this->tokens.empty() || !this->tokens.front().isQuote()) {
		throw PQLSyntaxError("Expected closing quote");
	}

	// Ignore '"' token
	this->tokens.pop_front();

	return ClauseArgument::createStringLiteralArg(token.getTokenString());
}

ClauseArgument ClauseParser::parseStatementNumber() {
	PQLToken stmtNumToken = this->tokens.front();
	this->tokens.pop_front();
	return ClauseArgument::createLineNumberArg(stmtNumToken.getTokenString());
}

ClauseArgument ClauseParser::parseWildcard() {
	PQLToken wildCardToken = this->tokens.front();
	this->tokens.pop_front();
	return ClauseArgument::createWildcardArg();
}

void ClauseParser::consumeOpenBracket() {
	if (this->tokens.empty() || !this->tokens.front().isOpenBracket()) {
		throw PQLSyntaxError("Expected Open bracket");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLSyntaxError("Expected argument after open bracket");
	}
}

void ClauseParser::consumeComma() {
	if (this->tokens.empty() || !this->tokens.front().isComma()) {
		throw PQLSyntaxError("Expected Comma");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLSyntaxError("Expected argument after comma");
	}
}

void ClauseParser::consumeCloseBracket() {
	if (this->tokens.empty() || !this->tokens.front().isCloseBracket()) {
		throw PQLSyntaxError("Expected Close bracket");
	}
	this->tokens.pop_front();
}

list<PQLToken> ClauseParser::getRemainingTokens() {
	assert(this->isParseCompleted);
	return this->tokens;
}
