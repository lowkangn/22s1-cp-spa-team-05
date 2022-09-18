#pragma once

#include <list>
#include <string>
#include <vector>
using namespace std;

#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/tokens/Token.h>


/*
	This class handles the parsing of a list of tokens into recursively-defined syntax rules, 
	which can then be used to validate the correctness and/or construct a syntax tree using 
	recursive descent.
*/
class ParserManager {
	
private: 
	shared_ptr<ASTNode> root; // root node of constructed AST
	list<Token> tokens; // tokens to parse

public:

	/*
		Constructs a parser manager. Expects the tokens to be parsed to be instantiated 
		with it at run time.
	*/
	ParserManager(list<Token> tokens) : tokens(tokens) {
		this->tokens = tokens;

		// AST node expects a token
		// convert linkedlist to vector
		this->root = shared_ptr<ASTNode> (new ProgramASTNode(this->tokens.front()));
	}

	/*
		Parses the tokens into an abstract syntax tree.
	*/
	shared_ptr<ASTNode> parse();
};