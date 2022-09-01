#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/VariableNameSimpleSyntaxRule.h>
#include <list>

vector<SimpleSyntaxRule> ReadSimpleSyntaxRule::generateChildRules()
{
	vector<SimpleSyntaxRule> childRules; 
	// only child is variable
	VariableNameSimpleSyntaxRule rule = VariableNameSimpleSyntaxRule();
	rule.consumeTokens(this->tokens);
	childRules.push_back(childRules);
	return childRules;

}

list<Token> ReadSimpleSyntaxRule::consumeTokens(list<Token> tokens)
{
	// a read statement consists of a read token, a variable name token that is not a reserved word 
	// and a semicolon
	list<Token> childTokens;

	// read token
	Token token = tokens.front(); // read
	tokens.pop_front(); // pop
	if (!token.isReadKeywordToken()) {
		throw 
	}

	// variable token
	token = tokens.front();
	tokens.pop_front(); // pop
	if (!token.isNameToken()) {
		throw
	}
	childTokens.push_back(token);

	// is semicolon
	token = tokens.front();
	tokens.pop_front(); // pop
	if (!token.isClosedCurlyBracketToken()) {
		throw
	}

	// store the state
	this->tokens = childTokens;
	this->initialized = true;

	return tokens;
}
