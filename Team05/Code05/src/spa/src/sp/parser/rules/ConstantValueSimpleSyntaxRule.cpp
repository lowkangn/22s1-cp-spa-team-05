#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>


vector<shared_ptr<SimpleSyntaxRule>> ConstantValueSimpleSyntaxRule::generateChildRules() {
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	return vector<shared_ptr<SimpleSyntaxRule>>(); // terminal
}

list<Token> ConstantValueSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	tokens.pop_front();

	// should have exactly one token left
	if (!tokens.empty()) {
		throw SimpleSyntaxParserException("Expected exactly one token left for constant!");
	}

	// token should be a integer token
	if (!token.isIntegerToken()) {
		throw SimpleSyntaxParserException("Token should be an integer!");
	}

	// set state
	list<Token> childTokens;
	childTokens.push_back(token);
	this->initialized = true;
	this->tokens = childTokens;
	return tokens; // now empty
}


ASTNode ConstantValueSimpleSyntaxRule::constructNode() {
	// TODO
	return ASTNode();
}