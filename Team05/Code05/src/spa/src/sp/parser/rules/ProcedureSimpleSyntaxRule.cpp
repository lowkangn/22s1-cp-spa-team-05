#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>

#include <string>

const int PROCEDURE = 0;
const int STMT_LST = 1;

vector<shared_ptr<SimpleSyntaxRule>> ProcedureSimpleSyntaxRule::generateChildRules() {
    // must be initialized
    if (!this->initialized) {
        throw SimpleSyntaxRuleNotInitializedException();
    }

    vector<shared_ptr<SimpleSyntaxRule>> childRules;

    // first token should be a procedure name
    list<Token> tokens = this->tokens;
    shared_ptr<SimpleSyntaxRule> procedureNamePointer = make_shared<NameSimpleSyntaxRule>();
    tokens = procedureNamePointer->consumeTokens(tokens);
    childRules.push_back(procedureNamePointer);

    // a procedure is defined by a statement list, pass remaining into that
    shared_ptr<SimpleSyntaxRule> statementListPointer = make_shared<
        StatementListSimpleSyntaxRule>();
    tokens = statementListPointer->consumeTokens(tokens);
    childRules.push_back(statementListPointer);

    if (!tokens.empty()) {
        throw SimpleSyntaxParserException("Should have passed all tokens into statement list!");
    }

    return childRules;
}

list<Token> ProcedureSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    list<Token> consumedTokens;

    // a procedure consists of the procedure keyword and name
    // then statements sandwiched by two {} tokens
    // first token should be procedure keyword
    Token token = tokens.front();
    if (!token.isProcedureKeywordToken()) {
        throw SimpleSyntaxParserException(
            string("Expected first token to be procedure keyword, but was ") + token.getString());
    }
    tokens.pop_front(); // is ok, let's pop the top token out

    // second token should be a name
    token = tokens.front(); // read
    if (!token.isNameToken()) {
        throw SimpleSyntaxParserException(
            string("Expected second token to be valid name, but was ") + token.getString());
    }
    tokens.pop_front(); // pop
    consumedTokens.push_back(token);

    // third token should be {
    token = tokens.front();
    if (!token.isOpenCurlyBracketToken()) {
        throw SimpleSyntaxParserException(
            string("Expected third token to be open bracket, but was ") + token.getString());
    }
    tokens.pop_front(); // is ok, let's pop the top token out
    consumedTokens.push_back(token);

    // then we keep going until we hit a }
    bool seenCloseBracket = false;
    bool seenOneToken = false;
    int numOpenBracketSeen = 1;
    while (!tokens.empty()) {
        token = tokens.front(); // read
        tokens.pop_front(); // pop
        consumedTokens.push_back(token); // insert all tokens in order within bracket

        if (token.isOpenCurlyBracketToken()) {
            numOpenBracketSeen += 1;
        } else if (token.isClosedCurlyBracketToken()) {
            numOpenBracketSeen -= 1;
            if (numOpenBracketSeen == 0) {
                seenCloseBracket = true;
                break;
            }
        } else {
            seenOneToken = true;
        }
    }

    if (!seenOneToken) {
        throw SimpleSyntaxParserException("Procedure should have atleast one token inside");
    }

    // if no }, throw exception
    if (!seenCloseBracket) {
        throw SimpleSyntaxParserException("Missing closed bracket!");
    }
    // else, return remaining tokens and assign
    this->tokens = consumedTokens;
    this->initialized = true;

    return tokens;
}

shared_ptr<ASTNode> ProcedureSimpleSyntaxRule::constructNode() {
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // generate if needed
    if (!this->generated) {
        this->childRules = this->generateChildRules();
    }

    // create current node
    shared_ptr<ASTNode> procedureNameNode = this->childRules[PROCEDURE]->constructNode();
    Token token = procedureNameNode->getToken();
    shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(token);

    // create stmtlst node
    shared_ptr<ASTNode> stmtLstNode = this->childRules[STMT_LST]->constructNode();

    // Add child to procedure node to construct tree
    procedureNode->addChild(stmtLstNode);

    return procedureNode;
}
