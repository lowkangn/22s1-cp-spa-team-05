#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>

#include <string>

vector<shared_ptr<SimpleSyntaxRule>> ConstantValueSimpleSyntaxRule::generateChildRules() {
    // must be initialized
    if (!this->initialized) {
        throw SimpleSyntaxRuleNotInitializedException();
    }

    return vector<shared_ptr<SimpleSyntaxRule>>(); // terminal
}

list<Token> ConstantValueSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    // consume one token
    Token token = tokens.front();
    tokens.pop_front();

    // token should be a integer token
    if (!token.isIntegerToken()) {
        throw SimpleSyntaxParserException(
            string("Token should be an integer, but was: ") + token.getString());
    }

    this->checkNonZero(token);

    // set state
    list<Token> childTokens;
    childTokens.push_back(token);
    this->initialized = true;
    this->tokens = childTokens;
    return tokens;
}


shared_ptr<ASTNode> ConstantValueSimpleSyntaxRule::constructNode() {
    // check that initialized correctly
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // create current node
    Token constantValueToken = this->tokens.front();
    shared_ptr<ASTNode> constantValueNode = ConstantValueASTNode::createConstantNode(
        constantValueToken);

    return constantValueNode;
}

void ConstantValueSimpleSyntaxRule::checkNonZero(Token token) {
    if (token.getString().size() > 1) {
        if (token.getString()[0] == '0') {
            throw SimpleSyntaxParserException(
                string("Token's first digit should not be 0, but was: ") + token.getString());
        }
    }
}
