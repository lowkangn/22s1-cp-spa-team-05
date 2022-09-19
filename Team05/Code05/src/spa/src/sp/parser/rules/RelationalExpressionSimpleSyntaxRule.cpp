#include <sp/parser/rules/RelationalExpressionSimpleSyntaxRule.h>

using namespace std;

vector<shared_ptr<SimpleSyntaxRule>> RelationalExpressionSimpleSyntaxRule::generateChildRules() {
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	shared_ptr<SimpleSyntaxRule> firstRelationalFactor(new RelationalFactorSimpleSyntaxRule);
	firstRelationalFactor->consumeTokens(this->lhsTokens);

	shared_ptr<SimpleSyntaxRule> operatorSimpleSyntaxRule(new OperatorSimpleSyntaxRule());
	operatorSimpleSyntaxRule->consumeTokens(list<Token>{operatorToken});

	shared_ptr<SimpleSyntaxRule> secondRelationalFactor(new RelationalFactorSimpleSyntaxRule);
	secondRelationalFactor->consumeTokens(this->rhsTokens);

	vector<shared_ptr<SimpleSyntaxRule>> childRules { firstRelationalFactor , operatorSimpleSyntaxRule , secondRelationalFactor };

	return childRules;
}

list<Token> RelationalExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// consume one token
	Token token = tokens.front();
	list<Token> childTokens;

	int numOpenBracketSeen = 0;
	if (token.isOpenBracketToken()) {
		int numOpenBracketSeen = 1;
		tokens.pop_front();
	}

	while (!tokens.empty() && !tokens.front().isRelationalOperator()) {
		token = tokens.front();
		tokens.pop_front();
		if (token.isOpenBracketToken()) {
			numOpenBracketSeen += 1;
		} else if (token.isClosedBracketToken()) {
			numOpenBracketSeen -= 1;
			if (numOpenBracketSeen == 0) {
				break;
			}
		}
		this->lhsTokens.push_back(token);
	}



	if (tokens.empty()) {
		throw SimpleSyntaxParserException("Relational Expression must contain opeartor");
	}

	// get relational operator
	this->operatorToken = tokens.front();
	if (!this->operatorToken.isRelationalOperator()) {
		throw SimpleSyntaxParserException("Expected relational operator got:" + this->operatorToken.getString());
	}
	tokens.pop_front();


	// Parse the second relational factor
	token = tokens.front();
	numOpenBracketSeen = 1;
	while (!tokens.empty()) {
		token = tokens.front();
		tokens.pop_front();
		if (token.isOpenBracketToken()) {
			numOpenBracketSeen += 1;
		}
		else if (token.isClosedBracketToken()) {
			numOpenBracketSeen -= 1;
			if (numOpenBracketSeen == 0) {
				break;
			}
		}
		this->rhsTokens.push_back(token);
	}

	this->initialized = true;
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

	shared_ptr<SimpleSyntaxRule> lhsFactor = this->childRules[LHS_FACTOR];
	shared_ptr<ASTNode> leftHandSideNode = lhsFactor->constructNode();

	shared_ptr<SimpleSyntaxRule> operatorRule = this->childRules[RELATIONAL_OPERATOR];
	shared_ptr<ASTNode> expressionNode = operatorRule->constructNode();

	shared_ptr<SimpleSyntaxRule> rhsFactor = this->childRules[RHS_FACTOR];
	shared_ptr<ASTNode> rightHandSideNode = rhsFactor->constructNode();

	expressionNode->addChild(leftHandSideNode);
	expressionNode->addChild(rightHandSideNode);

	return expressionNode;
}