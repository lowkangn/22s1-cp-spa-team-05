#include <qps/query_parser/parsers/ClauseParser.h>

ClauseArgument ClauseParser::parseSynonym() {
    PQLToken synonymToken = this->tokens.front();
    if (!synonymToken.isName()) {
        throw PQLSyntaxError("Synonym expected, got: " + synonymToken.getTokenString());
    }
    this->tokens.pop_front();
    if (declarations.count(synonymToken.getTokenString()) == 0) {
        this->semanticErrorMessage = "Synonym not declared: " + synonymToken.getTokenString();
        return ClauseArgument::createUndeclaredSynonymArg(synonymToken.getTokenString());
    }
    return ClauseArgument::createArgument(
        synonymToken.getTokenString(),
        declarations.at(synonymToken.getTokenString()));
}

ClauseArgument ClauseParser::parseOneStmtRefOrEntRef() {
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
        throw PQLSyntaxError("Expected stmtRef or entRef, got: " + token.getTokenString());
    }
}

ClauseArgument ClauseParser::parseStringLiteral() {
    // Check '"' token
    PQLToken token = this->tokens.front();
    if (!token.isQuote()) {
        throw PQLSyntaxError("Quotation mark expected, got: " + token.getTokenString());
    }
    this->tokens.pop_front();

    token = this->tokens.front();
    if (!token.isName()) {
        throw PQLSyntaxError("Expected name in quotes, got: " + token.getTokenString());
    }
    this->tokens.pop_front();

    // Check '"' token
    if (this->tokens.empty() || !this->tokens.front().isQuote()) {
        throw PQLSyntaxError("Expected closing quote");
    }
    this->tokens.pop_front();

    return ClauseArgument::createStringLiteralArg(token.getTokenString());
}

ClauseArgument ClauseParser::parseStatementNumber() {
    PQLToken stmtNumToken = this->tokens.front();
    if (!stmtNumToken.isInteger()) {
        throw PQLSyntaxError("Integer expected, got: " + stmtNumToken.getTokenString());
    }
    this->tokens.pop_front();
    return ClauseArgument::createLineNumberArg(stmtNumToken.getTokenString());
}

ClauseArgument ClauseParser::parseWildcard() {
    PQLToken wildCardToken = this->tokens.front();
    if (!wildCardToken.isUnderscore()) {
        throw PQLSyntaxError("Wildcard expected, got: " + wildCardToken.getTokenString());
    }
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
