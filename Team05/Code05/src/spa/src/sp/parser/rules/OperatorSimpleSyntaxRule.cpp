#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>

#include <string>

vector<shared_ptr<SimpleSyntaxRule>> OperatorSimpleSyntaxRule::generateChildRules() {
    // must be initialized
    if (!this->initialized) {
        throw SimpleSyntaxRuleNotInitializedException();
    }

    return vector<shared_ptr<SimpleSyntaxRule>>(); // terminal
}

list<Token> OperatorSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    // consume one token
    Token token = tokens.front();
    tokens.pop_front();

    // token should be a operator token
    if (!token.isOperatorToken() && !token.isRelationalOperator() && !token.
        isConditionalOperator()) {
        throw SimpleSyntaxParserException(
            string("Token should be an operator, but was: ") + token.getString());
    }

    // set state
    list<Token> childTokens;
    childTokens.push_back(token);
    this->initialized = true;
    this->tokens = childTokens;
    return tokens;
}

shared_ptr<ASTNode> OperatorSimpleSyntaxRule::constructNode() {
    // check that initialized correctly
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // generate if needed
    if (!this->generated) {
        this->childRules = this->generateChildRules();
    }

    // create current node
    Token operatorToken = this->tokens.front();
    shared_ptr<ASTNode> ExpressionNode = ExpressionASTNode::createExpressionNode(operatorToken);

    return ExpressionNode;
}
