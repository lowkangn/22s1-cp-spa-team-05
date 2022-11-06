#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

#include <list>
#include <memory>
#include <string>
#include <vector>


using namespace std;
/*
	This class handles the parsing of a list of tokens into recursively-defined syntax rules, 
	which can then be used to validate the correctness and/or construct a syntax tree using
	recursive descent.
*/
class ParserManager {
private:
    shared_ptr<ASTNode> root; // root node of constructed AST

public:
    /*
        Constructs a parser manager. Expects the tokens to be parsed to be instantiated
        with it at run time.
    */
    ParserManager() {
        // AST node expects a token
        // convert linkedlist to vector
        this->root = ProgramASTNode::createProgramNode();
    }

    /*
        Parses the tokens into an abstract syntax tree.
    */
    shared_ptr<ASTNode> parse(list<Token> tokens);
};
