#include <sp/parser/rules/VariableNameSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>


vector<SimpleSyntaxRule> VariableNameSimpleSyntaxRule::generateChildRules() {
	return vector<SimpleSyntaxRule>(); // terminal
}

list<Token> VariableNameSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	tokens.pop_front();

	// should have exactly one token left
	if (!tokens.empty()) {
		SimpleSyntaxParserException("Expected exactly one token left for variable name!")
	}

	// token should be a name token
	if (!token.isNameToken()) {
		SimpleSyntaxParserException("Token should be a variable name!")
	}

	// set state
	list<Token> childTokens;
	childTokens.push_back(token);
	this->initialized = true;
	this->tokens = childTokens
}
