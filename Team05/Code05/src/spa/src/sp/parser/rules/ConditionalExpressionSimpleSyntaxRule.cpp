#include <sp/parser/rules/ConditionalExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/RelationalExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
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

const int FIRST_RULE = 0;
const int SECOND_RULE = 1;

vector<shared_ptr<SimpleSyntaxRule>> ConditionalExpressionSimpleSyntaxRule::generateChildRules() {
	
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	vector<shared_ptr<SimpleSyntaxRule>> childRules;
	list<Token> tokens = this->tokens;
	
	Token token = tokens.front();

	if (token.isNotOperator()) {
		shared_ptr<SimpleSyntaxRule> operatorRulePointer = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		tokens = operatorRulePointer.consume(tokens);
		this->setOperatorRule(operatorRulePointer);

		shared_ptr<SimpleSyntaxRule> conditionalRulePointer = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		tokens = conditionalRulePointer.consume(tokens);
		childRules.push_back(conditionalRulePointer)
	}
	else if (token.isOpenBracketToken()) {
		// First rule
		shared_ptr<SimpleSyntaxRule> firstConditionalRulePointer = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		tokens = firstConditionalRulePointer.consume(tokens);
		childRules.push_back(firstConditionalRulePointer);

		shared_ptr<SimpleSyntaxRule> operatorRulePointer = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		tokens = operatorRulePointer.consume(tokens);
		this->setOperatorRule(operatorRulePointer);

		// Second rule
		shared_ptr<SimpleSyntaxRule> secondConditionalRulePointer = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		tokens = secondConditionalRulePointer.consume(tokens);
		childRules.push_back(secondConditionalRulePointer);
	}
	else if (token.isNameToken()) {
		shared_ptr<SimpleSyntaxRule> relationalRulePointer = shared_ptr<SimpleSyntaxRule>(new RelationalExpressionSimpleSyntaxRule());
		tokens = relationalRulePointer.consume(tokens);
		childRules.push_back(relationalRulePointer);
	}
	else {
		throw SimpleSyntaxParserException("Invalid token in Conditional Expression Rule ")
	}

	return childRules;
}

list<Token> ConditionalExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	bool isNotToken = false;

	// There are multiple relational expressions
	if (token.isOpenBracketToken() || token.isNotOperator()) {
		list<Token> childTokens;
		
		// Pop first relational expression
		if (token.isNotOperator()) {
			// Place Not operator back
			childTokens.push_back(token);
			bool isNotToken = true;
			tokens.pop_front();
		}

		token = token.front();
		// Validate first expression
		if (token.isOpenBracketToken()) {
			bool seenCloseBracket = false;
			while (!seenCloseBracket || !tokens.empty()) {
				token = tokens.front();
				tokens.pop_front();

				if (token.isClosedBracketToken()) {
					seenCloseBracket = true;
					break;
				}
				childTokens.push_back(token);
			}
			if (!seenCloseBracket) {
				throw SimpleSyntaxParserException("Missing closed bracket!")
			}
		}
		else {
			throw SimpleSyntaxParserException("Missing open bracket!");
		}

		// If no not token seen there is a operator and another conditional expression
		if (!isNotToken) {
			// Pop operator next
			token = tokens.front();
			if (!token.isConditionalOperator()) {
				throw SimpleSyntaxParserException("Expected conditional operator in expression but got " + token.getString());
			}
			tokens.pop_front();
			childTokens.push_back(token);


			bool seenCloseBracket = false;

			// validate the other relational expression
			token = tokens.front();
			tokens.pop_front();
			if (!token.isOpenBracketToken()) {
				throw SimpleSyntaxParserException("Conditional expression in conditional expressions should start with (");
			}
			bool seenCloseBracket = false;
			while (!seenCloseBracket || !tokens.empty()) {
				token = tokens.front();
				tokens.pop_front();

				if (token.isClosedBracketToken()) {
					seenCloseBracket = true;
					break;
				}
				childTokens.push_back(token);
			}

			if (!seenCloseBracket) {
				throw SimpleSyntaxParserException("Missing closed bracket!");
			}
		}
	}
	// Only one relational expression
	else {
		this->tokens = tokens;
	}

	this->initialized = true;
	this->tokens = childTokens;
	return tokens;
}

shared_ptr<ASTNode> ConditionalExpressionSimpleSyntaxRule::constructNode() {
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
		this->operatorRule->generateChildRules();
	}

	shared_ptr<SimpleSyntaxRule> firstCond = this->childRules[FIRST_RULE];
	shared_ptr<SimpleSyntaxRule> secondCond = this->childRules[SECOND_RULE];
	shared_ptr<SimpleSyntaxRule> operatorRule = this->getOperatorRule();

	shared_ptr<ASTNode> leftHandSideNode = firstCond->constructNode();
	shared_ptr<ASTNode> rightHandSideNode = secondCond->constructNode();
	shared_ptr<ASTNode> operatorNode = operatorRule->constructNode();

	operatorNode->addChild(leftHandSideNode);
	operatorNode->addChild(rightHandSideNode);

	return operator;
}