#include <sp/parser/rules/ExpressionSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
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
	vector<shared_ptr<SimpleSyntaxRule>> childRules;

	// lhs
	list<Token> lhsTokens = this->lhsTokens;

	// More than one term on the lhs, thus it is still an expression
	if (lhsTokens.size() == 1) {
		if (lhsTokens.front().isNameToken()) {
			shared_ptr<SimpleSyntaxRule> lhsRulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
			lhsTokens = lhsRulePointer->consumeTokens(lhsTokens); // consume the tokens
			childRules.push_back(lhsRulePointer);
		}
		else if (lhsTokens.front().isIntegerToken()) {
			shared_ptr<SimpleSyntaxRule> lhsRulePointer = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
			lhsTokens = lhsRulePointer->consumeTokens(lhsTokens); // consume the tokens
			childRules.push_back(lhsRulePointer);
		}
		else {
			throw SimpleSyntaxParserException("LHS of an expression must be another expression or a variable/constant");
		}
	}
	// Means lhs is either variable or constant
	else {
		shared_ptr<SimpleSyntaxRule> lhsRulePointer = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		childRules.push_back(lhsRulePointer);
	}
	
	Token operatorToken = this->operatorToken;
	// Have to put in list as the constructor for SimpleSyntaxRule takes in a list<Token>
	list<Token> operatorTokenList;
	operatorTokenList.push_back(operatorToken);
	// Middle must be an operator
	if (!operatorToken.isOperatorToken()) {
		throw SimpleSyntaxParserException("Second token should be a operator!");
	}
	shared_ptr<SimpleSyntaxRule> operatorRulePointer = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
	operatorTokenList = operatorRulePointer->consumeTokens(operatorTokenList); // consume the tokens
	childRules.push_back(operatorRulePointer);

	// RHS can be expression or a constant/variable
	list<Token> rhsTokens = this->rhsTokens;
	// if there is more than one token left then it is not terminal and another expression
	if (rhsTokens.size() == 1) {
		// Either a Name or a Constant
		if (rhsTokens.front().isNameToken()) {
			shared_ptr<SimpleSyntaxRule> nameRulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
			rhsTokens = nameRulePointer->consumeTokens(rhsTokens); // consume the tokens
			childRules.push_back(nameRulePointer);
		}
		else if (rhsTokens.front().isIntegerToken()) {
			shared_ptr<SimpleSyntaxRule> constantValueRulePointer = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
			rhsTokens = constantValueRulePointer->consumeTokens(rhsTokens); // consume the tokens
			childRules.push_back(constantValueRulePointer);
		}
		else {
			throw SimpleSyntaxParserException("Unrecognized token in the RHS of the expression");
		}
	}
	else {
		shared_ptr<SimpleSyntaxRule> expressionRulePointer = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		rhsTokens = expressionRulePointer->consumeTokens(rhsTokens); // consume the tokens
		childRules.push_back(expressionRulePointer);
	}

	// should have no tokens left
	if (!rhsTokens.empty() && !lhsTokens.empty()) {
		throw SimpleSyntaxParserException("Should have no remaining tokens after parsing assign statement, but still has!");
	}
	return childRules;
}

list<Token> ExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {

	list<Token> lhsTokens;
	list<Token> rhsTokens;

	Token token = tokens.front(); // read


	// Check for brackets, if they are present means first term itself is an expression (Factor)
	if (token.isOpenBracketToken()) {
		lhsTokens = this->extractBracketTokens(tokens);
		this->lhsBrackets = true;
	}
	else {
		// should start with variable or constant
		if (!token.isNameToken() && !token.isIntegerToken()) {
			throw SimpleSyntaxParserException(string("Expected a name variable or constant, got: ") + token.getString());
		}
		lhsTokens.push_back(token); // we want the lhs variable
		tokens.pop_front(); // pop
	}

	// Then operator
	token = tokens.front(); // read
	if (!token.isOperatorToken()) {
		throw SimpleSyntaxParserException(string("Expected an operator got: ") + token.getString());
	}
	tokens.pop_front(); // pop
	this->operatorToken = token; // we want the operator

	// Check for brackets, if they are present then the second term itself is an expression (factor)
	token = tokens.front();
	if (token.isOpenBracketToken()) {
		rhsTokens = this->extractBracketTokens(tokens);
		this->rhsBrackets = true;
	}
	// It is a term
	else {
		// should start with variable or constant
		token = tokens.front();
		tokens.pop_front(); // pop
		if (!token.isNameToken() && !token.isIntegerToken()) {
			throw SimpleSyntaxParserException(string("Expected a name variable or constant, got: ") + token.getString());
		}
		rhsTokens.push_back(token); // we want the lhs variable

		// Append the rest of the expression
		while (!tokens.empty()) {
			// then operator 
			token = tokens.front(); // read
			tokens.pop_front(); // pop
			rhsTokens.push_back(token);
		}
	}
	
	// then store state
	this->lhsTokens = lhsTokens;
	this->rhsTokens = rhsTokens;
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

	shared_ptr<ASTNode> lhsBrackets(new BracketsASTNode(Token::getPlaceHolderToken()));
	shared_ptr<ASTNode> rhsBrackets(new BracketsASTNode(Token::getPlaceHolderToken()));

	if (this->lhsBrackets) {
		lhsBrackets->addChild(lhsNode);
		operatorNode->addChild(lhsBrackets);
	}
	else {
		operatorNode->addChild(lhsNode); // LHS must be added before RHS
	}

	if (this->rhsBrackets) {
		rhsBrackets->addChild(rhsNode);
		operatorNode->addChild(rhsBrackets);
	}
	else {
		operatorNode->addChild(rhsNode);
	}

	return operatorNode;
}

list<Token> ExpressionSimpleSyntaxRule::extractBracketTokens(list<Token> &tokens) {

	list<Token> insideBracketTokens;

	Token token = tokens.front();
	tokens.pop_front();

	if (!token.isOpenBracketToken()) {
		throw SimpleSyntaxParserException("Cannot extract bracket tokens as it does not start with a bracket '('");
	}

	int numOpenBracket = 1;
	bool seenClosedBracket = false;
	while (!tokens.empty() && !seenClosedBracket) {
		token = tokens.front();
		tokens.pop_front();

		if (token.isOpenBracketToken()) {
			numOpenBracket += 1;
		}
		else if (token.isClosedBracketToken()) {
			numOpenBracket -= 1;
			if (numOpenBracket == 0) {
				seenClosedBracket = true;
				break;
			}
		}
		insideBracketTokens.push_back(token);
	}

	return insideBracketTokens;
}