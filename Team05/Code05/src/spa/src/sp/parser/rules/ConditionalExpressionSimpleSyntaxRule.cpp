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

const int CONDITIONAL_OPERATOR_RULE = 0;
const int FIRST_RULE = 1;
const int SECOND_RULE = 2;

vector<shared_ptr<SimpleSyntaxRule>> ConditionalExpressionSimpleSyntaxRule::generateChildRules() {
	
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	vector<shared_ptr<SimpleSyntaxRule>> childRules;
	list<Token> tokens = this->tokens;
	
	Token token = tokens.front();

	if (this->notOperatorUsed) {
		shared_ptr<SimpleSyntaxRule> conditionalRulePointer = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		tokens = conditionalRulePointer->consumeTokens(tokens);
		childRules.push_back(conditionalRulePointer);
	}
	else if (this->twoConditionals) {
		// First rule
		shared_ptr<SimpleSyntaxRule> firstConditionalRulePointer = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		tokens = firstConditionalRulePointer->consumeTokens(tokens);
		childRules.push_back(firstConditionalRulePointer);

		// Operator rule
		shared_ptr<SimpleSyntaxRule> operatorRulePointer = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		tokens = operatorRulePointer->consumeTokens(tokens);
		childRules.push_back(operatorRulePointer);

		// Second rule
		shared_ptr<SimpleSyntaxRule> secondConditionalRulePointer = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		tokens = secondConditionalRulePointer->consumeTokens(tokens);
		childRules.push_back(secondConditionalRulePointer);
	}
	else if (token.isNameToken()) {
		shared_ptr<SimpleSyntaxRule> relationalRulePointer = shared_ptr<SimpleSyntaxRule>(new RelationalExpressionSimpleSyntaxRule());
		tokens = relationalRulePointer->consumeTokens(tokens);
		childRules.push_back(relationalRulePointer);
	}
	else {
		throw SimpleSyntaxParserException("Invalid token in Conditional Expression Rule ");
	}

	return childRules;
}

list<Token> ConditionalExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();

	list<Token> childTokens;
	// It is a not operator encompassing a conditional expression
	if (token.isNotOperator()) {
		// Pop first relational expression
		if (token.isNotOperator()) {
			// Do not place Not operator back
			this->notOperatorUsed = true;
			tokens.pop_front();
		}

		// Get conditional expr
		childTokens = parseCondition(tokens);
	}
	// It is a standard conditional expression (cond_expr) OPERATOR (cond_expr)
	else if (token.isOpenBracketToken()) {
		// Get First Conditional Expression
		list<Token> firstCondExpression = parseCondition(tokens);
		childTokens.insert(childTokens.end(), firstCondExpression.begin(), firstCondExpression.end());

		// Get Conditional Operator
		token = tokens.front();
		tokens.pop_front();
		if (!token.isConditionalOperator()) {
			throw SimpleSyntaxParserException("Conditional operator expected got:" + token.getString());
		}

		childTokens.push_back(token);

		// Get Second Conditional Expression
		list<Token> secondCondExpression = parseCondition(tokens);
		childTokens.insert(childTokens.end(), firstCondExpression.begin(), firstCondExpression.end());

		this->twoConditionals = true;
	}
	else {
		// Only one relational statement
		Token token = tokens.front();
		tokens.pop_front();

		// token should be a name token
		if (!token.isNameToken()) {
			throw SimpleSyntaxParserException("Token should be a variable name!");
		}

		// set state
		childTokens.push_back(token);

		// Next token should be a relational operator
		token = tokens.front();
		tokens.pop_front();

		if (!token.isRelationalOperator()) {
			throw SimpleSyntaxParserException("Token should be a relational operator!");
		}

		childTokens.push_back(token);

		// Last token should be a variable
		token = tokens.front();
		tokens.pop_front();

		// token should be a name token
		if (!token.isNameToken() && !token.isIntegerToken()) {
			throw SimpleSyntaxParserException("Token should be a variable name or a constant token!");
		}

		childTokens.push_back(token);
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
	}

	// Single relational term/ConditionalRule from Not operator
	if (this->childRules.size() == 1) {
		shared_ptr<SimpleSyntaxRule> childRule = this->childRules[0];
		return childRule->constructNode();
	} else {
		shared_ptr<SimpleSyntaxRule> operatorRule = this->childRules[CONDITIONAL_OPERATOR_RULE];
		shared_ptr<SimpleSyntaxRule> firstCond = this->childRules[FIRST_RULE];
		shared_ptr<SimpleSyntaxRule> secondCond = this->childRules[SECOND_RULE];

		shared_ptr<ASTNode> leftHandSideNode = firstCond->constructNode();
		shared_ptr<ASTNode> rightHandSideNode = secondCond->constructNode();
		shared_ptr<ASTNode> operatorNode = operatorRule->constructNode();

		operatorNode->addChild(leftHandSideNode);
		operatorNode->addChild(rightHandSideNode);
		return operatorNode;
	}
}

list<Token> ConditionalExpressionSimpleSyntaxRule::parseCondition(list<Token> &tokens) {

	list<Token> childTokens;

	Token token = tokens.front();
	if (!token.isOpenBracketToken()) {
		throw SimpleSyntaxParserException("Conditional expressions must start with a (");
	}
	tokens.pop_front();

	bool seenCloseBracket = false;
	int numOpenBracket = 1;
	// Get conditional expression inside the brackets
	while (!tokens.empty() && !seenCloseBracket) {
		token = tokens.front();
		tokens.pop_front();

		if (token.isOpenBracketToken()) {
			numOpenBracket += 1;
		}
		else if (token.isClosedBracketToken()) {
			numOpenBracket -= 1;
			if (numOpenBracket == 0) {
				seenCloseBracket = true;
				break;
			}
		}
		childTokens.push_back(token);
	}
	if (!seenCloseBracket) {
		throw SimpleSyntaxParserException("Missing closed bracket!");
	}
	return childTokens;
}

