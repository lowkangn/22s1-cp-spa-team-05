#pragma once

#include <list>
#include <string>
using namespace std;

#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/tokens/Token.h>


/*
	This class handles the parsing of a list of tokens into recursively-defined syntax rules, 
	which can then be used to validate the correctness and/or construct a syntax tree using 
	recursive descent.
*/
class ParserManager {
	
private: 
	ASTNode *root; // root node of constructed AST
	list<Token> tokens; // tokens to parse

public:

	/*
		Constructs a parser manager. Expects the tokens to be parsed to be instantiated 
		with it at run time.
	*/
	ParserManager(vector<Token> tokens) {
		this->tokens = tokens;
		this->root = new ASTNode(tokens);
	}

	/*
		Parses the tokens into an abstract syntax tree.
	*/
	ASTNode parse();
};