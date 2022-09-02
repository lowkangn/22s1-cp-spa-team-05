#include <sp/parser/rules/ConstantSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>


vector<SimpleSyntaxRule> ConstantSimpleSyntaxRule::generateChildRules() {
	return vector<SimpleSyntaxRule>(); // terminal
}

list<Token> ConstantSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	tokens.pop_front();

	// should have exactly one token left
	if (!tokens.empty()) {
		SimpleSyntaxParserException("Expected exactly one token left for constant!")
	}

	// token should be a integer token
	if (!token.isIntegerToken()) {
		SimpleSyntaxParserException("Token should be an integer!")
	}

	// set state
	list<Token> childTokens;
	childTokens.push_back(token);
	this->initialized = true;
	this->tokens = childTokens;
	return tokens; // now empty
}
