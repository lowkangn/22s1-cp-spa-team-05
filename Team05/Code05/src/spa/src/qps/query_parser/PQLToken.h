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

};

