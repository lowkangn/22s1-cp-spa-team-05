#pragma once

#include <string>

using namespace std;

/**
 * Class of enumerators for the different types of PQLTokens.
 */
enum class PQLTokenType {
    NAME,
    INTEGER,
    OPERATOR,
    DELIMITER
};

// keywords that form the select-cl
const string SELECT = "Select";
const string SUCH = "such";
const string THAT = "that";
const string PATTERN = "pattern";

// delimiters
const string SEMICOLON = ";";
const string COMMA = ",";
const string OPEN_BRACKET = "(";
const string CLOSE_BRACKET = ")";
const string QUOTE = "\"";
const string UNDERSCORE = "_";

/**
 * This class represents tokens obtained from tokenizing of the query string.
 */
class PQLToken {

private:

    /**
     * String representation of the token.
     */
    string tokenString;

    /**
     * Type of the token.
     */
    PQLTokenType tokenType;

public:

    /**
     *
     * @param tokenString
     * @param tokenType
     */
    PQLToken(string tokenString, PQLTokenType tokenType) {
        this->tokenString = tokenString;
        this->tokenType = tokenType;
    }

    /**
     * Returns the string representation of the token.
     *
     * @return string representation of the token.
     */
    string getTokenString() {
        return tokenString;
    }

    /**
     * Returns the PQLTokenType.
     *
     * @return the PQLTokenType.
     */
    PQLTokenType getTokenType() {
        return tokenType;
    }

    /**
     * Tests if another token is equal to this token, i.e. same tokenString and tokenType.
     * @param otherToken to test.
     * @return true if tokens are equal.
     */
    bool equals(PQLToken otherToken) {
        return tokenString == otherToken.getTokenString() && tokenType == otherToken.getTokenType();
    }

    bool isSelect() {
        return tokenType == PQLTokenType::NAME && tokenString == SELECT;
    }

    bool isSuch() {
        return tokenType == PQLTokenType::NAME && tokenString == SUCH;
    }

    bool isThat() {
        return tokenType == PQLTokenType::NAME && tokenString == THAT;
    }

    bool isPattern() {
        return tokenType == PQLTokenType::NAME && tokenString == PATTERN;
    }

    bool isSemicolon() {
        return tokenType == PQLTokenType::DELIMITER && tokenString == SEMICOLON;
    }

    bool isComma() {
        return tokenType == PQLTokenType::DELIMITER && tokenString == COMMA;
    }

    bool isOpenBracket() {
        return tokenType == PQLTokenType::DELIMITER && tokenString == OPEN_BRACKET;
    }

    bool isCloseBracket() {
        return tokenType == PQLTokenType::DELIMITER && tokenString == CLOSE_BRACKET;
    }

    bool isQuote() {
        return tokenType == PQLTokenType::DELIMITER && tokenString == QUOTE;
    }

    bool isUnderscore() {
        return tokenType == PQLTokenType::DELIMITER && tokenString == UNDERSCORE;
    }

    bool isName() {
        return tokenType == PQLTokenType::NAME;
    }
};

