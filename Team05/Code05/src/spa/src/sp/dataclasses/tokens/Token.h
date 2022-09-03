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


// delimiters
const string OPEN_CURLY_BRACKET = "{";
const string CLOSED_CURLY_BRACKET = "}";
const string SEMI_COLON = ";";

// operators
const string EQUAL_OPERATOR = "=";

/*
	Encapsulates a token for backend use.
*/
class Token {
private:
	string s;
	TokenType type;
	
public: 
	Token(string s, TokenType type) {
		this->s = s;
		this->type = type;
	}

	TokenType getType() {
		return this->type;
	}
	string getString() {
		return this->s;
	}

	bool equals(Token other) {
		return ((this->getType() == other.getType()) && (this->getString() == other.getString()));
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

	/* Checks if the token is the call keyword.*/
	bool isCallKeywordToken() {
		return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == CLOSED_CURLY_BRACKET);
	}

	/* Checks if the token is the while keyword */
	bool isWhileKeywordToken() {
		return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == WHILE_KEYWORD);
	}

	/* Checks if the token is an if keyword.*/
	bool isIfKeywordToken() {
		return (this->type == TokenType::NAME_OR_KEYWORD) && (this->s == IF_KEYWORD);
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
		return (this->isProcedureKeywordToken() 
				|| this->isReadKeywordToken()
				|| this->isCallKeywordToken()
				|| this->isWhileKeywordToken()
				|| this->isIfKeywordToken()
			);
	}

	// ===== Delimiter identifier ======
	/* Checks if the token is an open bracket. */
	bool isOpenCurlyBracketToken() {
		return (this->type == TokenType::DELIMITER) && (this->s == OPEN_CURLY_BRACKET);
	}

	/* Checks if the token is a closed curly bracket.*/
	bool isClosedCurlyBracketToken() {
		return (this->type == TokenType::DELIMITER) && (this->s == CLOSED_CURLY_BRACKET);
	}

	/* Checks if the token is a closed curly bracket.*/
	bool isSemiColonToken() {
		return (this->type == TokenType::DELIMITER) && (this->s == SEMI_COLON);
	}

	// ====== Number identifier =====
	bool isIntegerToken() {
		return (this->type == TokenType::INTEGER);
	}

};
