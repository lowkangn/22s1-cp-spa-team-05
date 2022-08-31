#pragma once
#include <string>
using namespace std;

enum class TokenType {
	NAME,
	INTEGER,
	OPERATOR,
	DELIMITER,
	INVALID
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
	
};
