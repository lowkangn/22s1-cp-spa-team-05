#include <qps/query_parser/parsers/SuchThatClauseParser.h>

pair<ClauseArgument, ClauseArgument> SuchThatClauseParser::extractArguments() {
	// check '('
	consumeClauseOpen();

	// get first arg and check ','
	ClauseArgument firstArg = parseOneArgument();
	consumeClauseMiddle();

	// get second arg and check ')'
	ClauseArgument secondArg = parseOneArgument();
	consumeClauseClose();
	
	return make_pair(firstArg, secondArg);
}

ClauseArgument SuchThatClauseParser::parseOneArgument() {
	PQLToken token = this->tokens.front();
	if (token.isName()) {
		return parseSynonym();
	} else if (token.isQuote()) {
		return parseStringLiteral();
	} else if (token.isInteger()) {
		return parseStatementNumber();
	} else if (token.isUnderscore()) {
		return parseWildcard();
	} else {
		throw PQLError("Expected stmtRef or entRef, got: " + token.getTokenString());
	}
}
