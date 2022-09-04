#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/AST.h>
#include <list>
#include <memory>

using namespace std;

const int LHS = 0;
const int RHS = 1;

vector<shared_ptr<SimpleSyntaxRule>> AssignSimpleSyntaxRule::generateChildRules() {
	// should be initialized 
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	// initialize outputs
	list<Token> tokens = this->tokens;
	vector<shared_ptr<SimpleSyntaxRule>> childRules;

	// variable name on lhs 
	if (!tokens.front().isNameToken()) {
		throw SimpleSyntaxParserException("First token should be a variable!");
	}
	shared_ptr<SimpleSyntaxRule> lhsRulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
	tokens = lhsRulePointer->consumeTokens(tokens); // consume the tokens
	childRules.push_back(lhsRulePointer);

	// expression rule on rhs, consume remaining tokens
	// NOTE: for MVP we only allow constant assignment
	// TODO: do a proper expression
	shared_ptr<SimpleSyntaxRule> rhsRulePointer = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
	tokens = rhsRulePointer->consumeTokens(tokens);
	childRules.push_back(rhsRulePointer);

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
	if (!token.isNameToken()) {
		throw SimpleSyntaxParserException(string("Expected a name variable, got: ") + token.getString());
	}
	childTokens.push_back(token); // we want the lhs variable
	

	// then equals 
	token = tokens.front(); // read
	if (!token.isEqualToken()) {
		throw SimpleSyntaxParserException(string("Expected a = operator, got: ") + token.getString());
	}
	tokens.pop_front(); // pop

	// then tokens until semicolon delimiter (rhs stuff)
	bool seenSemiColon = false;
	bool seenOneToken = false;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop

		if (token.isSemiColonToken()) {
			seenSemiColon = true;
			break;
		}
		childTokens.push_back(token);
		seenOneToken = true;
	}
	if (!seenSemiColon || !seenOneToken) {
		throw SimpleSyntaxParserException("No terminal semicolon or too few tokens before, invalid assign statement!");
	}

	// then store state
	this->tokens = childTokens;
	this->initialized = true;
	return tokens; // return remaining tokens
}

shared_ptr<ASTNode> AssignSimpleSyntaxRule::constructNode() {
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// create current node
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	shared_ptr<ASTNode> assignNode(new ASTNode(vector<Token>{assignToken}));
	assignNode->setType(ASTNodeType::ASSIGN);


	shared_ptr<ASTNode> leftHandSide = this->childRules[LHS]->constructNode();
	leftHandSide->setType(ASTNodeType::NAME);

	// NOTE: for MVP we only allow constant assignment
	// TODO: do a proper expression
	shared_ptr<ASTNode> rightHandSide = this->childRules[RHS]->constructNode();
	rightHandSide->setType(ASTNodeType::CONSTANT);

	assignNode->addChild(leftHandSide);
	assignNode->addChild(rightHandSide);

	return assignNode;
}
