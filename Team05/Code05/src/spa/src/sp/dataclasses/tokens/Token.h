#pragma once
#include <string>
using namespace std;

/*
	The possible token types. We group them as such as this way balances ease of 
	parsing character-by-character while retaining sufficiently logical groups 
	for parsing into more abstract representations (e.g. a syntax tree).
*/
enum class TokenType {
    NAME_OR_KEYWORD,
    INTEGER,
    OPERATOR,
    DELIMITER,
    INVALID,
};

/*
	Here, we define some keywords.
*/
// keywords
const string PROCEDURE_KEYWORD = "procedure";
const string READ_KEYWORD = "read";
const string CALL_KEYWORD = "call";
const string WHILE_KEYWORD = "while";
const string IF_KEYWORD = "if";
const string THEN_KEYWORD = "then";
const string ELSE_KEYWORD = "else";
const string PROGRAM_KEYWORD = "program";
const string PRINT_KEYWORD = "print";


// delimiters
const string OPEN_CURLY_BRACKET = "{";
const string CLOSED_CURLY_BRACKET = "}";
const string CLOSED_BRACKET = ")";
const string OPEN_BRACKET = "(";
const string SEMI_COLON = ";";

// operators
const string EQUAL_OPERATOR = "=";
const string PLUS_OPERATOR = "+";
const string MINUS_OPERATOR = "-";
const string MULTIPLY_OPERATOR = "*";
const string DIVIDE_OPERATOR = "/";
const string MODULUS_OPERATOR = "%";

// relational operators
const string GREATER_THAN_OPERATOR = ">";
const string GREATER_THAN_EQUAL_OPERATOR = ">=";
const string LESS_THAN_OPERATOR = "<";
const string LESS_THAN_EQUAL_OPERATOR = "<=";
const string EQUALITY_OPERATOR = "==";
const string NOT_EQUAL_OPERATOR = "!=";

// Conditional operators
const string NOT = "!";
const string AND = "&&";
const string OR = "||";


/*
	Encapsulates a token for backend use.
*/
class Token {
private:
    string s;
    TokenType type;

    Token(string s, TokenType type) {
        this->s = s;
        this->type = type;
    }

public:
    static Token createNameOrKeywordToken(string s) {
        return Token(s, TokenType::NAME_OR_KEYWORD);
    }

    static Token createReadToken() {
        return Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createPrintToken() {
        return Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createCallToken() {
        return Token(CALL_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createWhileToken() {
        return Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createIfToken() {
        return Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createThenToken() {
        return Token(THEN_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createElseToken() {
        return Token(ELSE_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createProgramToken() {
        return Token(PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createProcedureToken() {
        return Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD);
    }

    static Token createDelimiterToken(string s) {
        return Token(s, TokenType::DELIMITER);
    }

    static Token createSemicolonToken() {
        return Token(SEMI_COLON, TokenType::DELIMITER);
    }

    static Token createOpenBracketToken() {
        return Token(OPEN_BRACKET, TokenType::DELIMITER);
    }

    static Token createCloseBracketToken() {
        return Token(CLOSED_BRACKET, TokenType::DELIMITER);
    }

    static Token createOpenCurlyBracketToken() {
        return Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER);
    }

    static Token createCloseCurlyBracketToken() {
        return Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER);
    }

    static Token createIntegerToken(string s) {
        return Token(s, TokenType::INTEGER);
    }

    static Token createPlaceholderToken() {
        return Token("", TokenType::INVALID);
    }

    static Token createOperatorToken(string s) {
        return Token(s, TokenType::OPERATOR);
    }

    static Token createEqualsToken() {
        return Token(EQUAL_OPERATOR, TokenType::OPERATOR);
    }

    static Token createNotEqualsToken() {
        return Token(NOT_EQUAL_OPERATOR, TokenType::OPERATOR);
    }

    static Token createPlusToken() {
        return Token(PLUS_OPERATOR, TokenType::OPERATOR);
    }

    static Token createMinusToken() {
        return Token(MINUS_OPERATOR, TokenType::OPERATOR);
    }

    static Token createMultiplyToken() {
        return Token(MULTIPLY_OPERATOR, TokenType::OPERATOR);
    }

    static Token createDivideToken() {
        return Token(DIVIDE_OPERATOR, TokenType::OPERATOR);
    }

    static Token createModulusToken() {
        return Token(MODULUS_OPERATOR, TokenType::OPERATOR);
    }

    static Token createLessThanToken() {
        return Token(LESS_THAN_OPERATOR, TokenType::OPERATOR);
    }

    static Token createGreaterThanToken() {
        return Token(GREATER_THAN_OPERATOR, TokenType::OPERATOR);
    }

    static Token createEqualityToken() {
        return Token(EQUALITY_OPERATOR, TokenType::OPERATOR);
    }

    static Token createGreaterThanEqualToken() {
        return Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
    }

    static Token createNotToken() {
        return Token(NOT, TokenType::OPERATOR);
    }

    static Token createAndToken() {
        return Token(AND, TokenType::OPERATOR);
    }

    static Token createOrToken() {
        return Token(OR, TokenType::OPERATOR);
    }

    TokenType getType() {
        return this->type;
    }

    string getString() {
        return this->s;
    }

    static Token getPlaceHolderToken() {
        return Token{"", TokenType::INVALID};
    }

    bool operator==(Token other) const {
        return ((this->type == other.getType()) && (this->s == other.getString()));
    }

    // ===== Keyword helper methods =====

    /* Checks if a token is the procedure keyword. */
    bool isProcedureKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == PROCEDURE_KEYWORD);
    }

    /* Checks if the token is the read keyword.*/
    bool isReadKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == READ_KEYWORD);
    }

    /* Checks if the token is the print keyword.*/
    bool isPrintKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == PRINT_KEYWORD);
    }

    /* Checks if the token is the call keyword.*/
    bool isCallKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == CALL_KEYWORD);
    }

    /* Checks if the token is the while keyword */
    bool isWhileKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == WHILE_KEYWORD);
    }

    /* Checks if the token is an if keyword.*/
    bool isIfKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == IF_KEYWORD);
    }

    /* Checks if the token is an then keyword.*/
    bool isThenKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == THEN_KEYWORD);
    }

    /* Checks if the token is an else keyword.*/
    bool isElseKeywordToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == ELSE_KEYWORD);
    }

    /* Checks if the token is a name token.*/
    bool isNameToken() {
        return (this->type == TokenType::NAME_OR_KEYWORD); // note that keywords can be names
    }

    /* Checks if the token is an equal sign.*/
    bool isEqualToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == EQUAL_OPERATOR);
    }

    /* Check that token is a reserved keyword.*/
    bool isKeywordToken() {
        return this->isProcedureKeywordToken()
            || this->isReadKeywordToken()
            || this->isCallKeywordToken()
            || this->isWhileKeywordToken()
            || this->isIfKeywordToken();
    }

    // ===== Delimiter identifier ======
    /* Checks if the token is an open curly bracket. */
    bool isOpenCurlyBracketToken() {
        return (this->type == TokenType::DELIMITER) && (this->s == OPEN_CURLY_BRACKET);
    }

    /* Checks if the token is a closed curly bracket.*/
    bool isClosedCurlyBracketToken() {
        return (this->type == TokenType::DELIMITER) && (this->s == CLOSED_CURLY_BRACKET);
    }

    /* Checks if the token is an open bracket. */
    bool isOpenBracketToken() {
        return (this->type == TokenType::DELIMITER) && (this->s == OPEN_BRACKET);
    }

    /* Checks if the token is a closed bracket.*/
    bool isClosedBracketToken() {
        return (this->type == TokenType::DELIMITER) && (this->s == CLOSED_BRACKET);
    }

    /* Checks if the token is a closed curly bracket.*/
    bool isSemiColonToken() {
        return (this->type == TokenType::DELIMITER) && (this->s == SEMI_COLON);
    }

    // ====== Number identifier =====
    bool isIntegerToken() {
        return (this->type == TokenType::INTEGER);
    }

    bool isPlusToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == PLUS_OPERATOR);
    }

    bool isMinusToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == MINUS_OPERATOR);
    }

    bool isMultiplyToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == MULTIPLY_OPERATOR);
    }

    bool isDivideToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == DIVIDE_OPERATOR);
    }

    bool isModulusToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == MODULUS_OPERATOR);
    }

    bool isOperatorToken() {
        return this->isPlusToken()
            || this->isMinusToken()
            || this->isMultiplyToken()
            || this->isDivideToken()
            || this->isModulusToken();
    }

    bool isGreaterToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == GREATER_THAN_OPERATOR);
    }

    bool isGreaterThanEqualToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == GREATER_THAN_EQUAL_OPERATOR);
    }

    bool isLesserToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == LESS_THAN_OPERATOR);
    }

    bool isLesserThanEqualToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == LESS_THAN_EQUAL_OPERATOR);
    }

    bool isEqualityToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == EQUALITY_OPERATOR);
    }

    bool isNotEqualToken() {
        return (this->type == TokenType::OPERATOR) && (this->s == NOT_EQUAL_OPERATOR);
    }

    bool isRelationalOperator() {
        return (this->isGreaterToken() || this->isGreaterThanEqualToken() || this->isLesserToken()
            || this->isLesserThanEqualToken() || this->isEqualityToken() || this->
            isNotEqualToken());
    }

    bool isNotOperator() {
        return (this->type == TokenType::OPERATOR) && (this->s == NOT);
    }

    bool isAndOperator() {
        return (this->type == TokenType::OPERATOR) && (this->s == AND);
    }

    bool isOrOperator() {
        return (this->type == TokenType::OPERATOR) && (this->s == OR);
    }

    bool isConditionalOperator() {
        return (this->isAndOperator() || this->isOrOperator());
    }
};
