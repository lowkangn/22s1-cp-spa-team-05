#pragma once

#include <list>
#include <vector>
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

/*
	This is the parent class for all simple syntax rules. The idea behind this class is that it encapsulates 
	the recursive generation and validation of rules.
*/
class SimpleSyntaxRule {
protected:
	list<Token> tokens;
	vector<SimpleSyntaxRule> childRules;
	bool generated; // boolean variable to make sure a generation attempt has been made
	bool initialized; // boolean variable to make sure tokens are not empty



public:

	/*
		Constructor initiliazes boolean flags to false. Flags are changed by the corresponding
		methods - generate child rules flips generated, create flips initialized.
	*/
	SimpleSyntaxRule() {
		this->generated = false;
		this->initialized = false;
	}

	/*
		This method causes the rule to create a new set of rules based on the internal tokens 
		it keeps track of and recursively validate them. An exception is thrown with a validation error.
	*/
	void validate() final;

	/*
		This method constructs a node out of the rule as part of an abstract syntax tree.
	*/
	ASTNode constructNode() final;

	/*
		This method generates a list of rules that are recursively defined based on the rule itself.
	*/
	virtual vector<SimpleSyntaxRule> generateChildRules()=0;

	/*
		This method, provided tokens, selects the relevant tokens to wrap into the rule object and 
		returns a new Rule object with said tokens, and returns the new linkedlist from which to continue
		reading the tokens.

		If it can be parsed, then the linkedlist returned will be shortened. 
		If not, it will be the same.


	*/
	virtual list<Token> consumeTokens(list<Token> tokens)=0;



};