#include <sp/parser/rules/ExpressionSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <list>
#include <memory>

using namespace std;

const int OPERATOR_RULE = 1;
const int LHS_RULE = 0;
const int RHS_RULE = 2;

vector<shared_ptr<SimpleSyntaxRule>> ExpressionSimpleSyntaxRule::generateChildRules() {
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

	// Middle must be an operator
	if (!tokens.front().isOperatorToken()) {
		throw SimpleSyntaxParserException("Second token should be a operator!");
	}
	shared_ptr<SimpleSyntaxRule> operatorRulePointer = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
	tokens = operatorRulePointer->consumeTokens(tokens); // consume the tokens
	childRules.push_back(operatorRulePointer);

	// RHS can be nested or the end of an expression
	// if there is more than one token left then it is not terminal and another expression
	if (tokens.size() == 1) {
		// Either a Name or a Constant
		if (tokens.front().isNameToken()) {
			shared_ptr<SimpleSyntaxRule> nameRulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
			tokens = nameRulePointer->consumeTokens(tokens); // consume the tokens
			childRules.push_back(nameRulePointer);
		}
		else {
			shared_ptr<SimpleSyntaxRule> constantValueRulePointer = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
			tokens = constantValueRulePointer->consumeTokens(tokens); // consume the tokens
			childRules.push_back(constantValueRulePointer);
		}
	}
	else {
		shared_ptr<SimpleSyntaxRule> expressionRulePointer = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		tokens = expressionRulePointer->consumeTokens(tokens); // consume the tokens
		childRules.push_back(expressionRulePointer);
	}

	// should have no tokens left
	if (!tokens.empty()) {
		throw SimpleSyntaxParserException("Should have no remaining tokens after parsing assign statement, but still has!");
	}
	return childRules;
}

list<Token> ExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {

	list<Token> childTokens;

	// should start with variable or constant
	Token token = tokens.front(); // read
	tokens.pop_front(); // pop
	if (!token.isNameToken() && !token.isIntegerToken()) {
		throw SimpleSyntaxParserException(string("Expected a name variable or constant, got: ") + token.getString());
	}
	childTokens.push_back(token); // we want the lhs variable

	while (!tokens.empty()) {
		// then operator 
		token = tokens.front(); // read
		if (!token.isOperatorToken()) {
			throw SimpleSyntaxParserException(string("Expected a operator, got: ") + token.getString());
		}
		tokens.pop_front(); // pop
		childTokens.push_back(token); // we want the operator

		// Another variable or constant
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		if (!token.isNameToken() && !token.isIntegerToken()) {
			throw SimpleSyntaxParserException(string("Expected a name variable or constant, got: ") + token.getString());
		}
		childTokens.push_back(token); // we want the lhs variable
	}
	
	// then store state
	this->tokens = childTokens;
	this->initialized = true;
	return tokens; // return remaining tokens
}

shared_ptr<ASTNode> ExpressionSimpleSyntaxRule::constructNode() {
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	shared_ptr<ASTNode> operatorNode = this->childRules[OPERATOR_RULE]->constructNode();
	shared_ptr<ASTNode> lhsNode = this->childRules[LHS_RULE]->constructNode();
	shared_ptr<ASTNode> rhsNode = this->childRules[RHS_RULE]->constructNode();

	operatorNode->addChild(lhsNode);
	operatorNode->addChild(rhsNode);

	return operatorNode;
}