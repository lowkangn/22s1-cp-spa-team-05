#include <qps/query_parser/PQLError.h>
#include <qps/query_parser/parsers/ClauseParser.h>

list<PQLToken> ClauseParser::getRemainingTokens() {
	return this->tokens;
}

ClauseArgument ClauseParser::parseSynonym() {
	PQLToken synonymToken = tokens.front();
	if (declarations.count(synonymToken.getTokenString()) == 0) {
		throw PQLError("Synonym not declared: " + synonymToken.getTokenString());
	}
	tokens.pop_front();
	if (isStatementDesignEntity(declarations.at(synonymToken.getTokenString()))) {
		return ClauseArgument(synonymToken.getTokenString(), ArgumentType::STMTREF_SYNONYM);
	} else {
		return ClauseArgument(synonymToken.getTokenString(), ArgumentType::ENTREF_SYNONYM);
	}
}

ClauseArgument ClauseParser::parseStringLiteral() {
	// Ignore '"' token
	tokens.pop_front();

	PQLToken token = tokens.front();
	if (!token.isName()) {
		throw PQLError("Expected name in quotes, got: " + token.getTokenString());
	}
	tokens.pop_front();
	if (tokens.empty() || !tokens.front().isQuote()) {
		throw PQLError("Expected closing quote");
	}
	tokens.pop_front();
	return ClauseArgument(token.getTokenString(), ArgumentType::STRING_LITERAL);
}

ClauseArgument ClauseParser::parseStatementNumber() {
	PQLToken stmtNumToken = tokens.front();
	tokens.pop_front();
	return ClauseArgument(stmtNumToken.getTokenString(), ArgumentType::LINE_NUMBER);
}

ClauseArgument ClauseParser::parseWildcard() {
	PQLToken wildCardToken = tokens.front();
	tokens.pop_front();
	return ClauseArgument(wildCardToken.getTokenString(), ArgumentType::WILDCARD);
}

void ClauseParser::consumeClauseOpen() {
	if (tokens.empty() || !tokens.front().isOpenBracket()) {
		throw PQLError("Expected Open bracket");
	}
	tokens.pop_front();
	if (tokens.empty()) {
		throw PQLError("Expected argument after open bracket");
	}
}

void ClauseParser::consumeClauseMiddle() {
	if (tokens.empty() || !tokens.front().isComma()) {
		throw PQLError("Expected Comma");
	}
	tokens.pop_front();
	if (tokens.empty()) {
		throw PQLError("Expected argument after comma");
	}
}

void ClauseParser::consumeClauseClose() {
	if (tokens.empty() || !tokens.front().isCloseBracket()) {
		throw PQLError("Expected Close bracket");
	}
	tokens.pop_front();
}