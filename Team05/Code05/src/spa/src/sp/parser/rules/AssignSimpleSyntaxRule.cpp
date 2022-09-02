#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/AST.h>
#include <list>
#include <memory>

using namespace std;

using namespace std;
vector<shared_ptr<SimpleSyntaxRule>> AssignSimpleSyntaxRule::generateChildRules() {
	// should be initialized 
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	// initialize outputs
	list<Token> tokens = this->tokens;
	vector<shared_ptr<SimpleSyntaxRule>> childRules;

	// variable name on lhs 
	shared_ptr<SimpleSyntaxRule> lhsRulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
	tokens = lhsRulePointer->consumeTokens(tokens);
	childRules.push_back(lhsRulePointer);

	// expression rule on rhs
	// NOTE: for MVP we only allow constant assignment
	// TODO: do a proper expression
	ConstantValueSimpleSyntaxRule rhsRule = ConstantValueSimpleSyntaxRule();
	tokens = rhsRule.consumeTokens(tokens);

	// should have no tokens left
	if (!tokens.empty()) {
		throw SimpleSyntaxParserException("Should have no remaining tokens after parsing assign statement, but still has!");
	}
	return childRules;

}

list<Token> AssignSimpleSyntaxRule::consumeTokens(list<Token> tokens) {

	list<Token> childTokens;

	// should start with variable 
	Token token = tokens.front(); // read
	tokens.pop_front(); // pop
	childTokens.push_back(token); // we want the lhs variable
	if (!token.isNameToken()) {
		// throw
	}

	// then equals 
	token = tokens.front(); // read
	tokens.pop_front(); // pop
	if (!token.isEqualToken()) {
		throw SimpleSyntaxParserException("Statement starting with variable should be assign statement, but no equal sign!");
	}

	// then tokens until semicolon delimiter (rhs stuff)
	bool seenSemiColon = false;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		childTokens.push_back(token);

		if (token.isSemiColonToken()) {
			seenSemiColon = true;
			break;
		}
	}
	if (!seenSemiColon) {
		throw SimpleSyntaxParserException("No terminal semicolon, invalid assign statement!");
	}

	// then store state
	this->tokens = childTokens;
	this->initialized = true;
	return tokens; // return remaining tokens
}

ASTNode AssignSimpleSyntaxRule::constructNode() {
	// TODO
	return ASTNode();
}
