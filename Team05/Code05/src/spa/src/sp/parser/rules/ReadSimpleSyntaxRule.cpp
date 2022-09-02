#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

#include <list>

vector<shared_ptr<SimpleSyntaxRule>> ReadSimpleSyntaxRule::generateChildRules()
{
	vector<shared_ptr<SimpleSyntaxRule>> childRules;
	// only child is variable
	shared_ptr<SimpleSyntaxRule> rulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
	rulePointer->consumeTokens(this->tokens);
	childRules.push_back(rulePointer);
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
		throw SimpleSyntaxParserException("first token should be a read token.");
	}

	// variable token
	token = tokens.front();
	tokens.pop_front(); // pop
	if (!token.isNameToken()) {
		throw SimpleSyntaxParserException("second token shouold be a name variable token.");
	}
	childTokens.push_back(token);

	// is semicolon
	token = tokens.front();
	tokens.pop_front(); // pop
	if (!token.isSemiColonToken()) {
		throw SimpleSyntaxParserException("third token should be a semicolon token.");
	}

	// store the state
	this->tokens = childTokens;
	this->initialized = true;

	return tokens;
}

ASTNode ReadSimpleSyntaxRule::constructNode() {
	// TODO
	return ASTNode();
}