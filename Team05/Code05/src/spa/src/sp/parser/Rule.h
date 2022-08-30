#pragma once

#include <vector>
#include <sp/dataclasses/ASTNode.h>
#include <sp/dataclasses/Token.h>

using namespace std;

class Rule {
private:
	vector<Token> tokens;
	vector<Rule> rules;
	bool generated = false;

public:

	/*
		This method causes the rule to create a new set of rules based on the internal tokens 
		it keeps track of and recursively validate them. An exception is thrown with a validation error.
	*/
	virtual void validate();

	/*
		This method generates a list of rules that are recursively defined based on the rule itself.
	*/
	virtual vector<Rule> generateRules();

	/*
		This method constructs a node out of the rule as part of an abstract syntax tree.
	*/
	virtual ASTNode constructNode();

	/*
		This method, provided tokens, selects the relevant tokens to wrap into the rule object and 
		returns a new Rule object with said tokens, and returns an integer offset from which to continue 
		reading the tokens.
	*/
	virtual int create(vector<Token> tokens, int start);

};