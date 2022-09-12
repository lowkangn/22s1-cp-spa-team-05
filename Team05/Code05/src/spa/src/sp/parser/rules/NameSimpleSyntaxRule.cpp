#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <vector>
using namespace std;

vector<shared_ptr<SimpleSyntaxRule>> NameSimpleSyntaxRule::generateChildRules() {
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	return vector<shared_ptr<SimpleSyntaxRule>>(); // terminal
}

list<Token> NameSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	tokens.pop_front();

	// token should be a name token
	if (!token.isNameToken()) {
		throw SimpleSyntaxParserException("Token should be a variable name!");
	}

	// set state
	list<Token> childTokens;
	childTokens.push_back(token);
	this->initialized = true;
	this->tokens = childTokens;
	return tokens; 
}


shared_ptr<ASTNode> NameSimpleSyntaxRule::constructNode() {
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	Token nameToken = this->tokens.front();
	shared_ptr<ASTNode> nameNode(new VariableASTNode(nameToken));

	return nameNode;
}
