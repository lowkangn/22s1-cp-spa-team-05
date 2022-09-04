#include <qps/query_parser/PQLError.h>
#include <qps/query_parser/parsers/ClauseParser.h>

list<PQLToken> ClauseParser::getRemainingTokens() {
	return this->tokens;
}

ClauseArgument ClauseParser::parseSynonym(PQLToken synonymToken) {
	if (declarations.count(synonymToken.getTokenString()) == 0) {
		throw PQLError("Synonym not declared: " + synonymToken.getTokenString());
	}
	return ClauseArgument(synonymToken.getTokenString(), ArgumentType::SYNONYM);
}

ClauseArgument ClauseParser::parseStringLiteral(PQLToken strLiteralToken) {
	if (!strLiteralToken.isName()) {
		throw PQLError("Expected name in quotes, got: " + strLiteralToken.getTokenString());
	}
	return ClauseArgument(strLiteralToken.getTokenString(), ArgumentType::STRING_LITERAL);
}

ClauseArgument ClauseParser::parseStatementNumber(PQLToken stmtNumToken) {
	return ClauseArgument(stmtNumToken.getTokenString(), ArgumentType::LINE_NUMBER);
}