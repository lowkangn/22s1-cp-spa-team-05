#pragma once

// imported libraries
#include "string"
#include "vector"
using namespace std;

// imported local files
#include "Token.h"

class Lexer {

private:

	// ===== attributes =====


public:
	// ===== methods =====
	/* 
		This method converts a string from a text file into a list of internally recognized tokens.
	 */
	vector<Token> tokenize(string fileText);
	
};