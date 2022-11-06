#include <sp/parser/rules/CallSimpleSyntaxRule.h>

vector<shared_ptr<SimpleSyntaxRule>> CallSimpleSyntaxRule::generateChildRules() {
    // must be initialized
    if (!this->initialized) {
        throw SimpleSyntaxRuleNotInitializedException();
    }

    return vector<shared_ptr<SimpleSyntaxRule>>(); // terminal
}

list<Token> CallSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    // a call statement consists of a call token,
    // a procedure name token that is not a reserved word,
    // and a semicolon
    list<Token> childTokens;

    // read is made up of 3 tokens
    if (tokens.size() < 3) {
        throw SimpleSyntaxParserException("incomplete call statement");
    }

    // call token
    Token token = tokens.front(); // read
    tokens.pop_front(); // pop
    if (!token.isCallKeywordToken()) {
        throw SimpleSyntaxParserException("first token should be a call token.");
    }

    // procedure token
    token = tokens.front();
    tokens.pop_front(); // pop
    if (!token.isNameToken()) {
        throw SimpleSyntaxParserException("second token should be a name procedure token.");
    }
    childTokens.push_back(token);

    // is semicolon
    token = tokens.front();
    tokens.pop_front(); // pop
    if (!token.isSemiColonToken()) {
        throw SimpleSyntaxParserException("third token should be a semicolon token.");
    }

    // store the state
    this->tokens = childTokens;
    this->initialized = true;

    return tokens;
}

shared_ptr<ASTNode> CallSimpleSyntaxRule::constructNode() {
    // check that initialized correctly
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // create call node
    shared_ptr<ASTNode> callNode = CallASTNode::createCallNode();

    // create child procedure node
    Token nameToken = this->tokens.front();
    shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(nameToken);
    shared_ptr<ASTNode> stmtListNode = StatementListASTNode::createStatementListNode();
    procedureNode->addChild(stmtListNode);

    callNode->addChild(procedureNode);

    return callNode;
}
