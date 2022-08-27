#pragma once
#include "string"
enum class TokenType {
};

class Token {
private:
	std::string s;
	TokenType type;
};