#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

#include <list>
#include <memory>
#include <vector>

using namespace std;

/*
	This is the parent class for all simple syntax rules. It encapsulates
	the recursive generation and validation of rules.
*/
class SimpleSyntaxRule {
protected:
    list<Token> tokens;
    vector<shared_ptr<SimpleSyntaxRule>> childRules;
    bool generated; // boolean variable to make sure a generation attempt has been made
    bool initialized; // boolean variable to make sure tokens are not empty

public:
    /*
        This method causes the rule to create a new set of rules based on the internal tokens
        it keeps track of and recursively validate them.
        An exception is thrown with a validation error.
    */
    virtual void validate();

    /*
        This method constructs a node out of the rule as part of an abstract syntax tree.
    */
    virtual shared_ptr<ASTNode> constructNode() = 0;

    /*
        This method generates rules that are recursively defined based on the rule itself.
    */
    virtual vector<shared_ptr<SimpleSyntaxRule>> generateChildRules() = 0;

    /*
        This method, provided tokens, selects the relevant tokens to wrap into the rule object,
        and returns the new linkedlist from which the caller can continue reading the tokens.

        If it can be parsed, then the linkedlist returned will be shortened. 
        If not, it will be the same.
    */
    virtual list<Token> consumeTokens(list<Token> tokens) = 0;

    /*
        Compares two rules to see if they are equal.
    */
    bool equals(SimpleSyntaxRule& other);
};
