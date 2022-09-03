#pragma once

#include <string>

using namespace std;

/**
 * Class of enumerators for the different types of PQLTokens.
 */
enum class PQLTokenType {};

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
     * Returns the string representation of the token.
     *
     * @return string representation of the token.
     */
    string getToken() {
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
};

