#include <sp/parser/rules/ConditionalExpressionSimpleSyntaxRule.h>


vector<shared_ptr<SimpleSyntaxRule>> ConditionalExpressionSimpleSyntaxRule::generateChildRules() {
    // must be initialized
    if (!this->initialized) {
        throw SimpleSyntaxRuleNotInitializedException();
    }

    vector<shared_ptr<SimpleSyntaxRule>> childRules;
    list<Token> tokens = this->tokens;

    // If not operator was found during consume tokens we process the inner expression recursively
    if (this->notOperatorUsed) {
        shared_ptr<SimpleSyntaxRule> conditionalRulePointer = make_shared<
            ConditionalExpressionSimpleSyntaxRule>();
        tokens = conditionalRulePointer->consumeTokens(tokens);
        childRules.push_back(conditionalRulePointer);
    } else if (this->twoConditionals) {
        // If two expressions were found during consume tokens,
        // process each expression as individual conditional expressions

        // First rule
        shared_ptr<SimpleSyntaxRule> firstConditionalRulePointer = make_shared<
            ConditionalExpressionSimpleSyntaxRule>();
        tokens = firstConditionalRulePointer->consumeTokens(this->lhsCond);
        childRules.push_back(firstConditionalRulePointer);

        // Operator rule
        shared_ptr<SimpleSyntaxRule> operatorRulePointer = make_shared<OperatorSimpleSyntaxRule>();
        tokens = operatorRulePointer->consumeTokens(list<Token>{condToken});
        childRules.push_back(operatorRulePointer);

        // Second rule
        shared_ptr<SimpleSyntaxRule> secondConditionalRulePointer = make_shared<
            ConditionalExpressionSimpleSyntaxRule>();
        tokens = secondConditionalRulePointer->consumeTokens(this->rhsCond);
        childRules.push_back(secondConditionalRulePointer);
    } else if (this->isRelationalExpression) {
        // If first token a name, process the ConditionalExpression as a relational expression
        shared_ptr<SimpleSyntaxRule> relationalRulePointer = make_shared<
            RelationalExpressionSimpleSyntaxRule>();
        tokens = relationalRulePointer->consumeTokens(tokens);
        childRules.push_back(relationalRulePointer);
    } else {
        throw SimpleSyntaxParserException("Invalid token in Conditional Expression Rule ");
    }

    return childRules;
}

list<Token> ConditionalExpressionSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    list<Token> childTokens;
    Token token = tokens.front();

    // It is a not operator encompassing a conditional expression
    if (token.isNotOperator()) {
        // Do not place Not operator back
        this->notOperatorUsed = true;
        tokens.pop_front();

        // Get conditional expr
        childTokens = this->parseCondition(tokens);
    } else if (token.isOpenBracketToken()) {
        // Get First Conditional Expression

        this->twoConditionals = this->isTwoConditional(tokens);

        if (this->twoConditionals) {
            list<Token> firstCondExpression = this->parseCondition(tokens);
            this->lhsCond = firstCondExpression;

            // Get Conditional Operator
            token = tokens.front();
            tokens.pop_front();
            if (!token.isConditionalOperator()) {
                throw SimpleSyntaxParserException(
                    "Conditional operator expected got:" + token.getString());
            }

            this->condToken = token;

            // Get Second Conditional Expression
            list<Token> secondCondExpression = this->parseCondition(tokens);
            this->rhsCond = secondCondExpression;
        } else {
            // only one conditional, so we know it is a relational expression
            this->isRelationalExpression = true;
            childTokens = this->parseRelationalExpression(tokens);
        }
    } else if (token.isNameToken() || token.isIntegerToken()) {
        this->isRelationalExpression = true;
        childTokens = tokens;
        tokens.clear();
    } else {
        throw SimpleSyntaxParserException("Found empty condition");
    }

    this->initialized = true;
    this->tokens = childTokens;

    if (!tokens.empty()) {
        throw SimpleSyntaxParserException(
            "Additional tokens provided to ConditionalExpressionSyntaxRule");
    }

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

    // Single relational expression
    if (this->childRules.size() == 1) {
        shared_ptr<SimpleSyntaxRule> childRule = this->childRules.front();
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


list<Token> ConditionalExpressionSimpleSyntaxRule::parseRelationalExpression(list<Token>& tokens) {
    list<Token> childTokens;

    bool seenRelationalOperator = false;

    Token token = tokens.front();

    // Get until the relational operator
    while (!tokens.empty() && !seenRelationalOperator) {
        token = tokens.front();
        tokens.pop_front();

        if (token.isRelationalOperator()) {
            seenRelationalOperator = true;
        }

        childTokens.push_back(token);
    }

    if (!seenRelationalOperator) {
        throw SimpleSyntaxParserException("Relational Operator not found in parsing expression");
    }

    // Get rest of the expression
    while (!tokens.empty()) {
        token = tokens.front();
        tokens.pop_front();
        childTokens.push_back(token);
    }

    return childTokens;
}

list<Token> ConditionalExpressionSimpleSyntaxRule::parseCondition(list<Token>& tokens) {
    list<Token> childTokens;

    Token token = tokens.front();

    int numOpenBracket = 1;
    tokens.pop_front();

    bool seenCloseBracket = false;
    // Get conditional expression inside the brackets
    while (!tokens.empty() && !seenCloseBracket) {
        token = tokens.front();
        tokens.pop_front();

        if (token.isOpenBracketToken()) {
            numOpenBracket += 1;
        } else if (token.isClosedBracketToken()) {
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


bool ConditionalExpressionSimpleSyntaxRule::isTwoConditional(list<Token> tokens) {
    if (!tokens.front().isOpenBracketToken()) {
        return false;
    }

    // Pass by value so list is copied
    list<Token> checkList{tokens};

    int numOpenBracketSeen = 0;
    int numOfTimeBracketWasOne = 0;
    bool seenCloseBracket = false;
    Token token = checkList.front();


    while (!checkList.empty() && !seenCloseBracket) {
        token = checkList.front();
        checkList.pop_front();

        if (token.isOpenBracketToken()) {
            numOpenBracketSeen += 1;
        } else if (token.isClosedBracketToken()) {
            numOpenBracketSeen -= 1;
        } else if (token.isConditionalOperator() && numOpenBracketSeen == 0) {
            return true;
        }
    }

    return false;
}
