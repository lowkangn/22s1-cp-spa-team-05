#pragma once
#include <string>
#include <unordered_map>
#include <regex>
using namespace std;

enum TokenType {
	NAME,
	INTEGER,
	OPERATOR,
	DELIMITER,
	INVALID;
};

/*
	Static data structure to stores the mapping of the tokentype to the 
	regex pattern to look out for. We initialize it as a static (as in, outside of 
	any class) variable to avoid duplicating repeatedly.
*/
unordered_map<TokenType, string> tokenTypeToRegexStringMap = {
	{TokenType::NAME, "^[a-zA-Z][a-zA-Z0-9]*"},
	{TokenType::INTEGER, "^[0-9]+"},
	{TokenType::OPERATOR, "[-+*\/]"},
	{TokenType::OPEN_BRACKET, "("},
	{TokenType::CLOSE_BRACKET, ")"},
	{TokenType::SEMI_COLON, ";"},
	{TokenType::INVALID, ""},
};


/*
	Encapsulates a token for backend use.
*/
class Token {
private:
	const string s;
	const TokenType type;
	

public: 

	Token(string s, TokenType type) {
		this.s = s;
		this.type = type;
	};
	
	/*
		Checks if the token instance is a valid one that can be parsed. 
		(e.g. 1n is never a valid token in SIMPLE)
	*/
	bool isValidToken() {
		return this.type == TokenType::INVALID
	}
	
};

class TokenFactory