#include <sp/parser/rules/RelationalFactorSimpleSyntaxRule.h>

vector<shared_ptr<SimpleSyntaxRule>> RelationalFactorSimpleSyntaxRule::generateChildRules() {
    vector<shared_ptr<SimpleSyntaxRule>> childRules;
    list<Token> tokens = this->tokens;

    if (tokens.size() > 1) {
        shared_ptr<SimpleSyntaxRule> expressionRulePointer = make_shared<
            ExpressionSimpleSyntaxRule>();
        list<Token> expressionTokens = expressionRulePointer->consumeTokens(this->tokens);
        childRules.push_back(expressionRulePointer);
    } else {
        if (tokens.front().isNameToken()) {
            // It is a variable
            shared_ptr<SimpleSyntaxRule> variableRulePointer = make_shared<NameSimpleSyntaxRule>();
            list<Token> nameToken = variableRulePointer->consumeTokens(tokens);
            // consume the tokens
            childRules.push_back(variableRulePointer);
        } else if (tokens.front().isIntegerToken()) {
            // It is a Constant value token
            shared_ptr<SimpleSyntaxRule> constantRulePointer = make_shared<
                ConstantValueSimpleSyntaxRule>();
            list<Token> constantToken = constantRulePointer->consumeTokens(tokens);
            // consume the tokens
            childRules.push_back(constantRulePointer);
        } else {
            throw SimpleSyntaxParserException(
                "RelationalFactor can only be variable/constant/expression");
        }
    }

    return childRules;
}

list<Token> RelationalFactorSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    // It is an expression if it has more than one child
    if (tokens.size() > 1) {
        this->tokens = tokens;
        tokens.clear();
    } else {
        if (tokens.front().isNameToken()) {
            // It is a variable
            Token nameVariable = tokens.front();
            tokens.pop_front();
            this->tokens.push_back(nameVariable);
        } else if (tokens.front().isIntegerToken()) {
            // It is a Constant value token
            Token constantToken = tokens.front();
            tokens.pop_front();
            this->tokens.push_back(constantToken);
        } else {
            throw SimpleSyntaxParserException(
                "RelationalFactor can only be variable/constant/expression");
        }
    }

    // There should be no tokens left
    if (!tokens.empty()) {
        throw SimpleSyntaxParserException(
            "There are still tokens left in the token list for RelationalFactor");
    }

    this->initialized = true;
    return tokens;
}

shared_ptr<ASTNode> RelationalFactorSimpleSyntaxRule::constructNode() {
    // check that initialized correctly
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // generate if needed
    if (!this->generated) {
        this->childRules = this->generateChildRules();
    }

    assert(this->childRules.size() == 1);
    shared_ptr<SimpleSyntaxRule> childRule = this->childRules.front();

    shared_ptr<ASTNode> relationFactorNode = childRule->constructNode();

    return relationFactorNode;
}
