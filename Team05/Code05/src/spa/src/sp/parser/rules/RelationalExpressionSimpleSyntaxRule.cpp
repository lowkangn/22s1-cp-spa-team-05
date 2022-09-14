#include <sp/parser/rules/RelationalExpressionSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <list>
#include <memory>

using namespace std;

vector<shared_ptr<SimpleSyntaxRule>> RelationalExpressionSimpleSyntaxRule::generateChildRules() {
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	return vector<shared_ptr<SimpleSyntaxRule>>(); // terminal
}

list<Token> RelationalExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	tokens.pop_front();

	// token should be a name token
	if (!token.isNameToken() && !token.isIntegerToken()) {
		throw SimpleSyntaxParserException("Token should be a variable name or a constant value!");
	}

	// set state
	list<Token> childTokens;
	childTokens.push_back(token);

	// Next token should be a relational operator
	token = tokens.front();
	tokens.pop_front();

	if (!token.isRelationalOperator()) {
		throw SimpleSyntaxParserException("Token should be a relational operator!");
	}

	childTokens.push_back(token);

	// Last token should be a variable or constant value
	token = tokens.front();
	tokens.pop_front();

	// token should be a name token
	if (!token.isNameToken() && !token.isIntegerToken()) {
		throw SimpleSyntaxParserException("Token should be a variable name or a constant token!");
	}

	childTokens.push_back(token);

	this->initialized = true;
	this->tokens = childTokens;
	return tokens;
}

shared_ptr<ASTNode> RelationalExpressionSimpleSyntaxRule::constructNode() {
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// create current node
	Token lhsToken = this->tokens.front();
	this->tokens.pop_front();
	Token relationalOperatorToken = this->tokens.front();
	this->tokens.pop_front();
	Token rhsToken = this->tokens.front();

	shared_ptr<ASTNode> expressionNode(new ExpressionASTNode(relationalOperatorToken));

	shared_ptr<ASTNode> leftHandSideNode;

	if (rhsToken.isIntegerToken()) {
		leftHandSideNode = shared_ptr<ASTNode>(new ConstantValueASTNode(lhsToken));
	}
	else if (rhsToken.isNameToken()) {
		leftHandSideNode = shared_ptr<ASTNode> (new VariableASTNode(lhsToken));
	}
	else {
		throw SimpleSyntaxParserException("Invalid left token found while generating ASTNode for RelationalExpressionSimpleSyntaxRule");
	}

	shared_ptr<ASTNode> rightHandSideNode;

	if (rhsToken.isIntegerToken()) {
		rightHandSideNode = shared_ptr<ASTNode>(new ConstantValueASTNode(rhsToken));
	}
	else if (rhsToken.isNameToken()) {
		rightHandSideNode = shared_ptr<ASTNode>(new VariableASTNode(rhsToken));
	}
	else {
		throw SimpleSyntaxParserException("Invalid right token found while generating ASTNode for RelationalExpressionSimpleSyntaxRule");
	}

	expressionNode->addChild(leftHandSideNode); // Left child added before right child
	expressionNode->addChild(rightHandSideNode);

	return expressionNode;
}