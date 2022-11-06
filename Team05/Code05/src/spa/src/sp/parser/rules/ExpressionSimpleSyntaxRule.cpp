#include <sp/parser/rules/ExpressionSimpleSyntaxRule.h>

#include <string>

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
    list<Token> lhsTokens = this->lhsTokens;
    list<Token> rhsTokens = this->rhsTokens;

    // More than one term on the lhs, thus it is still an expression
    if (!this->lhsIsExpression) {
        if (lhsTokens.front().isNameToken()) {
            shared_ptr<SimpleSyntaxRule> lhsRulePointer = make_shared<NameSimpleSyntaxRule>();
            lhsTokens = lhsRulePointer->consumeTokens(lhsTokens); // consume the tokens
            childRules.push_back(lhsRulePointer);
        } else if (lhsTokens.front().isIntegerToken()) {
            shared_ptr<SimpleSyntaxRule> lhsRulePointer = make_shared<
                ConstantValueSimpleSyntaxRule>();
            lhsTokens = lhsRulePointer->consumeTokens(lhsTokens); // consume the tokens
            childRules.push_back(lhsRulePointer);
        } else {
            throw SimpleSyntaxParserException(
                "LHS of an expression must be another expression or a variable/constant");
        }
    } else {
        // Means lhs is expression
        shared_ptr<SimpleSyntaxRule> lhsRulePointer = make_shared<ExpressionSimpleSyntaxRule>();
        lhsTokens = lhsRulePointer->consumeTokens(lhsTokens);
        childRules.push_back(lhsRulePointer);
    }

    if (!this->isSingleExpression) {
        Token operatorToken = this->operatorToken;
        // Have to put in list as the constructor for SimpleSyntaxRule takes in a list<Token>
        list<Token> operatorTokenList;
        operatorTokenList.push_back(operatorToken);
        // Middle must be an operator
        if (!operatorToken.isOperatorToken()) {
            throw SimpleSyntaxParserException("Second token should be a operator!");
        }
        shared_ptr<SimpleSyntaxRule> operatorRulePointer = make_shared<OperatorSimpleSyntaxRule>();
        operatorTokenList = operatorRulePointer->consumeTokens(operatorTokenList);
        // consume the tokens
        childRules.push_back(operatorRulePointer);

        // if there is more than one token left then it is not terminal and another expression
        if (rhsTokens.size() == 1) {
            // Either a Name or a Constant
            if (rhsTokens.front().isNameToken()) {
                shared_ptr<SimpleSyntaxRule> nameRulePointer = make_shared<NameSimpleSyntaxRule>();
                rhsTokens = nameRulePointer->consumeTokens(rhsTokens); // consume the tokens
                childRules.push_back(nameRulePointer);
            } else if (rhsTokens.front().isIntegerToken()) {
                shared_ptr<SimpleSyntaxRule> constantValueRulePointer = make_shared<
                    ConstantValueSimpleSyntaxRule>();
                rhsTokens = constantValueRulePointer->consumeTokens(rhsTokens);
                // consume the tokens
                childRules.push_back(constantValueRulePointer);
            } else {
                throw SimpleSyntaxParserException(
                    "Unrecognized token in the RHS of the expression");
            }
        } else {
            shared_ptr<SimpleSyntaxRule> expressionRulePointer = make_shared<
                ExpressionSimpleSyntaxRule>();
            rhsTokens = expressionRulePointer->consumeTokens(rhsTokens); // consume the tokens
            childRules.push_back(expressionRulePointer);
        }
    }


    // should have no tokens left
    if (!rhsTokens.empty() && !lhsTokens.empty()) {
        throw SimpleSyntaxParserException(
            "Should have no remaining tokens after parsing assign statement, but still has!");
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
        this->lhsIsExpression = true;
    } else {
        // should start with variable or constant
        if (!token.isNameToken() && !token.isIntegerToken()) {
            throw SimpleSyntaxParserException(
                string("Expected a name variable or constant, got: ") + token.getString());
        }
        lhsTokens.push_back(token); // we want the lhs variable
        tokens.pop_front(); // pop
    }

    if (tokens.empty()) {
        this->isSingleExpression = true;
    } else {
        // Means there is a second term
        // Then operator
        token = tokens.front(); // read
        if (!token.isOperatorToken()) {
            throw SimpleSyntaxParserException(
                string("Expected an operator got: ") + token.getString());
        }
        tokens.pop_front(); // pop
        this->operatorToken = token; // we want the operator

        // Check for brackets, if they are present then
        // the second term is a factor
        token = tokens.front();
        if (token.isOpenBracketToken()) {
            rhsTokens = this->extractBracketTokens(tokens);
            this->rhsIsExpression = true;
        } else {
            // It is a term
            // should start with variable or constant
            token = tokens.front();
            if (!token.isNameToken() && !token.isIntegerToken()) {
                throw SimpleSyntaxParserException(
                    string("Expected a name variable or constant, got: ") + token.getString());
            }

            rhsTokens = tokens;
            tokens.clear();
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


    if (isSingleExpression) {
        shared_ptr<ASTNode> expressionNode = this->childRules[LHS_RULE]->constructNode();
        if (this->lhsIsExpression) {
            shared_ptr<ASTNode> lhsBrackets = BracketsASTNode::createBracketsNode();
            lhsBrackets->addChild(expressionNode);
            return lhsBrackets;
        }
        return expressionNode;
    }

    shared_ptr<ASTNode> rootNode = this->childRules[OPERATOR_RULE]->constructNode();
    shared_ptr<ASTNode> lhsNode = this->childRules[LHS_RULE]->constructNode();
    shared_ptr<ASTNode> rhsNode = this->childRules[RHS_RULE]->constructNode();

    if (this->lhsIsExpression) {
        shared_ptr<ASTNode> lhsBrackets = BracketsASTNode::createBracketsNode();
        lhsBrackets->addChild(lhsNode);
        rootNode->addChild(lhsBrackets);
    } else {
        rootNode->addChild(lhsNode); // LHS must be added before RHS
    }

    if (this->rhsIsExpression) {
        shared_ptr<ASTNode> rhsBrackets = BracketsASTNode::createBracketsNode();
        rhsBrackets->addChild(rhsNode);
        rootNode->addChild(rhsBrackets);
    } else {
        rootNode->addChild(rhsNode);
    }
    return rootNode;
}

list<Token> ExpressionSimpleSyntaxRule::extractBracketTokens(list<Token>& tokens) {
    list<Token> insideBracketTokens;

    Token token = tokens.front();
    tokens.pop_front();

    if (!token.isOpenBracketToken()) {
        throw SimpleSyntaxParserException(
            "Cannot extract bracket tokens as it does not start with a bracket '('");
    }

    int numOpenBracket = 1;
    bool seenClosedBracket = false;
    while (!tokens.empty() && !seenClosedBracket) {
        token = tokens.front();
        tokens.pop_front();

        if (token.isOpenBracketToken()) {
            numOpenBracket += 1;
        } else if (token.isClosedBracketToken()) {
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
