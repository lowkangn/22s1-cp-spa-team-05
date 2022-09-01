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
	};
	TokenType getType() {
		return this->type;
	}
	string getString() {
		return this->s;
	}

	bool equals(Token other) {
		return ((this->getType() == other.getType()) && (this->getString() == other.getString()));
	}
};
