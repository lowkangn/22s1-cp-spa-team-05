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

// keywords for identifying Clause type
const string MODIFIES = "Modifies";
const string FOLLOW = "Follows";

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

    // tokenType indicators
    bool isName() {
        return tokenType == PQLTokenType::NAME;
    }
    bool isInteger() {
        return tokenType == PQLTokenType::INTEGER;
    }
    bool isDelimiter() {
        return tokenType == PQLTokenType::DELIMITER;
    }
    bool isOperator() {
        return tokenType == PQLTokenType::OPERATOR;
    }

    // Delimiter indicators
    bool isSemicolon() {
        return isDelimiter() && tokenString == SEMICOLON;
    }

    bool isComma() {
        return isDelimiter() && tokenString == COMMA;
    }

    bool isOpenBracket() {
        return isDelimiter() && tokenString == OPEN_BRACKET;
    }

    bool isCloseBracket() {
        return isDelimiter() && tokenString == CLOSE_BRACKET;
    }

    bool isQuote() {
        return isDelimiter() && tokenString == QUOTE;
    }

    bool isUnderscore() {
        return isDelimiter() && tokenString == UNDERSCORE;
    }


    // Query section indicators
    bool isSelect() {
        return isName() && tokenString == SELECT;
    }
    bool isSuch() {
        return isName() && tokenString == SUCH;
    }
    bool isThat() {
        return isName() && tokenString == THAT;
    }
    bool isPattern() {
        return isName() && tokenString == PATTERN;
    }

    // Clause Type indicators
    bool isModifies() {
        return isName() && tokenString == MODIFIES;
    }
};
