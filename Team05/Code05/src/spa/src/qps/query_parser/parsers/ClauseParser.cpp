#include <qps/query_parser/parsers/ClauseParser.h>

list<PQLToken> ClauseParser::getRemainingTokens() {
	return this->tokens;
}

ClauseArgument ClauseParser::parseSynonym() {
	PQLToken synonymToken = this->tokens.front();
	if (declarations.count(synonymToken.getTokenString()) == 0) {
		throw PQLError("Synonym not declared: " + synonymToken.getTokenString());
	}
	this->tokens.pop_front();
	if (isStatementDesignEntity(declarations.at(synonymToken.getTokenString()))) {
		return ClauseArgument(synonymToken.getTokenString(), ArgumentType::STMTREF_SYNONYM);
	} else {
		return ClauseArgument(synonymToken.getTokenString(), ArgumentType::ENTREF_SYNONYM);
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

void ClauseParser::consumeClauseOpen() {
	if (this->tokens.empty() || !this->tokens.front().isOpenBracket()) {
		throw PQLError("Expected Open bracket");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLError("Expected argument after open bracket");
	}
}

void ClauseParser::consumeClauseMiddle() {
	if (this->tokens.empty() || !this->tokens.front().isComma()) {
		throw PQLError("Expected Comma");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLError("Expected argument after comma");
	}
}

void ClauseParser::consumeClauseClose() {
	if (this->tokens.empty() || !this->tokens.front().isCloseBracket()) {
		throw PQLError("Expected Close bracket");
	}
	this->tokens.pop_front();
}