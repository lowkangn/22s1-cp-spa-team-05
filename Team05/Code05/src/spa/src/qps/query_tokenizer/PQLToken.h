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
const string WITH = "with";

// keywords for identifying Clause type
const string PQL_MODIFIES = "Modifies";
const string PQL_FOLLOWS = "Follows";
const string PQL_USES = "Uses";
const string PQL_PARENT = "Parent";
const string PQL_STAR_MODIFIER = "*";

// keywords for identifying attrName
const string ATTRIBUTE_PROCNAME = "procName";
const string ATTRIBUTE_VARNAME = "varName";
const string ATTRIBUTE_VALUE = "value";
const string ATTRIBUTE_STMT_NUM_STMT = "stmt";
const string ATTRIBUTE_STMT_NUM_HASH = "#";

// delimiters
const string SEMICOLON = ";";
const string COMMA = ",";
const string PQL_OPEN_BRACKET = "(";
const string PQL_CLOSED_BRACKET = ")";
const string QUOTE = "\"";
const string UNDERSCORE = "_";
const string HASH = "#";
const string DOT = ".";
const string EQUALS = "=";

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

    /**
     *
     * @param tokenString
     * @param tokenType
     */
    PQLToken(string tokenString, PQLTokenType tokenType) {
        this->tokenString = tokenString;
        this->tokenType = tokenType;
    }

public:
    // Factory methods
    static PQLToken createDelimiterToken(string tokenString) {
        return PQLToken(tokenString, PQLTokenType::DELIMITER);
    }

    static PQLToken createIntegerToken(string tokenString) {
        return PQLToken(tokenString, PQLTokenType::INTEGER);
    }

    static PQLToken createNameToken(string tokenString) {
        return PQLToken(tokenString, PQLTokenType::NAME);
    }

    static PQLToken createOperatorToken(string tokenString) {
        return PQLToken(tokenString, PQLTokenType::OPERATOR);
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
        return isDelimiter() && tokenString == PQL_OPEN_BRACKET;
    }

    bool isCloseBracket() {
        return isDelimiter() && tokenString == PQL_CLOSED_BRACKET;
    }

    bool isQuote() {
        return isDelimiter() && tokenString == QUOTE;
    }

    bool isUnderscore() {
        return isDelimiter() && tokenString == UNDERSCORE;
    }

    bool isDot() {
        return isDelimiter() && tokenString == DOT;
    }

    // Operator indicators
    bool isEquals() {
        return isOperator() && tokenString == EQUALS;
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
    bool isWith() {
        return isName() && tokenString == WITH;
    }

    // Clause Type indicators
    bool isModifies() {
        return isName() && tokenString == PQL_MODIFIES;
    }

    bool isUses() {
        return isName() && tokenString == PQL_USES;
    }

    bool isParent() {
        return isName() && tokenString == PQL_PARENT;
    }

    bool isFollows() {
        return isName() && tokenString == PQL_FOLLOWS;
    }

    bool isStarModifier() {
        return tokenString == PQL_STAR_MODIFIER;
    }

    // attrName idicators
    bool isProcName() {
        return isName() && tokenString == ATTRIBUTE_PROCNAME;
    }
    
    bool isVarName() {
        return isName() && tokenString == ATTRIBUTE_VARNAME;
    }
    
    bool isValue() {
        return isName() && tokenString == ATTRIBUTE_VALUE;
    }

    bool isStmtNumStmt() {
        return isName() && tokenString == ATTRIBUTE_STMT_NUM_STMT;
    }

    bool isStmtNumHash() {
        return isDelimiter() && tokenString == ATTRIBUTE_STMT_NUM_HASH;
    }
};

