#include <qps/query_parser/parsers/ClauseParser.h>

ClauseArgument ClauseParser::parseSynonym() {
    PQLToken synonymToken = this->tokens.front();
    if (declarations.count(synonymToken.getTokenString()) == 0) {
        throw PQLError("Synonym not declared: " + synonymToken.getTokenString());
    }
    this->tokens.pop_front();
    switch(declarations.at(synonymToken.getTokenString())) {
		case ArgumentType::STMT:
			return ClauseArgument::generateStmtArg(synonymToken.getTokenString());
		case ArgumentType::READ:
			return ClauseArgument::generateReadArg(synonymToken.getTokenString());
		case ArgumentType::PRINT:
			return ClauseArgument::generatePrintArg(synonymToken.getTokenString());
		case ArgumentType::ASSIGN:
			return ClauseArgument::generateAssignArg(synonymToken.getTokenString());
		case ArgumentType::CALL:
			return ClauseArgument::generateCallArg(synonymToken.getTokenString());
		case ArgumentType::WHILE:
			return ClauseArgument::generateWhileArg(synonymToken.getTokenString());
		case ArgumentType::IF:
			return ClauseArgument::generateIfArg(synonymToken.getTokenString());
		case ArgumentType::PROCEDURE:
			return ClauseArgument::generateProcedureArg(synonymToken.getTokenString());
		case ArgumentType::VARIABLE:
			return ClauseArgument::generateVariableArg(synonymToken.getTokenString());
		case ArgumentType::CONSTANT:
			return ClauseArgument::generateConstantArg(synonymToken.getTokenString());
		default:
			throw PQLError("parseSynonym: invalid ArgumentType");
	}
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
	return ClauseArgument::generateStringLiteralArg(token.getTokenString());
}

ClauseArgument ClauseParser::parseStatementNumber() {
	PQLToken stmtNumToken = this->tokens.front();
	this->tokens.pop_front();
	return ClauseArgument::generateLineNumberArg(stmtNumToken.getTokenString());
}

ClauseArgument ClauseParser::parseWildcard() {
	PQLToken wildCardToken = this->tokens.front();
	this->tokens.pop_front();
	return ClauseArgument::generateWildcardArg();
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
	if (!this->isParseCompleted) {
		throw PQLError("getRemainingTokens() called before parse()");
	}
	return this->tokens;
}
