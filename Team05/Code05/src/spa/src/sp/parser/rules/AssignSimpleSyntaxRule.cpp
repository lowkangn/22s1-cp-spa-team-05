#include <sp/parser/rules/AssignSimpleSyntaxRule.h>

#include <string>

const int LHS = 0;
const int RHS = 1;

vector<shared_ptr<SimpleSyntaxRule>> AssignSimpleSyntaxRule::generateChildRules() {
    // should be initialized
    if (!this->initialized) {
        throw SimpleSyntaxRuleNotInitializedException();
    }

    // initialize outputs
    list<Token> tokens = this->tokens;
    vector<shared_ptr<SimpleSyntaxRule>> childRules;

    // variable name on lhs
    if (!tokens.front().isNameToken()) {
        throw SimpleSyntaxParserException("First token should be a variable!");
    }
    shared_ptr<SimpleSyntaxRule> lhsRulePointer = make_shared<NameSimpleSyntaxRule>();
    tokens = lhsRulePointer->consumeTokens(tokens); // consume the tokens
    childRules.push_back(lhsRulePointer);

    // If more than one token left, it is an expression
    if (tokens.size() > 1) {
        // expression rule on rhs, consume remaining tokens
        shared_ptr<SimpleSyntaxRule> rhsRulePointer = make_shared<ExpressionSimpleSyntaxRule>();
        tokens = rhsRulePointer->consumeTokens(tokens);
        childRules.push_back(rhsRulePointer);
    } else {
        // Either a name token or a contant token
        if (tokens.front().isNameToken()) {
            shared_ptr<SimpleSyntaxRule> rhsRulePointer = make_shared<NameSimpleSyntaxRule>();
            tokens = rhsRulePointer->consumeTokens(tokens);
            childRules.push_back(rhsRulePointer);
        } else {
            shared_ptr<SimpleSyntaxRule> rhsRulePointer = make_shared<
                ConstantValueSimpleSyntaxRule>();
            tokens = rhsRulePointer->consumeTokens(tokens);
            childRules.push_back(rhsRulePointer);
        }
    }

    // should have no tokens left
    if (!tokens.empty()) {
        throw SimpleSyntaxParserException(
            "Should have no remaining tokens after parsing assign statement, but still has!");
    }
    return childRules;
}

list<Token> AssignSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    list<Token> childTokens;

    // should start with variable
    Token token = tokens.front(); // read
    tokens.pop_front(); // pop
    if (!token.isNameToken()) {
        throw SimpleSyntaxParserException(
            string("Expected a name variable, got: ") + token.getString());
    }
    childTokens.push_back(token); // we want the lhs variable


    // then equals
    token = tokens.front(); // read
    if (!token.isEqualToken()) {
        throw SimpleSyntaxParserException(
            string("Expected a = operator, got: ") + token.getString());
    }
    tokens.pop_front(); // pop

    // then tokens until semicolon delimiter (rhs stuff)
    bool seenSemiColon = false;
    bool seenOneToken = false;
    while (!tokens.empty()) {
        token = tokens.front(); // read
        tokens.pop_front(); // pop

        if (token.isSemiColonToken()) {
            seenSemiColon = true;
            break;
        }
        childTokens.push_back(token);
        seenOneToken = true;
    }

    if (!seenSemiColon || !seenOneToken) {
        throw SimpleSyntaxParserException(
            "No terminal semicolon or too few tokens before, invalid assign statement!");
    }

    // then store state
    this->tokens = childTokens;
    this->initialized = true;
    return tokens; // return remaining tokens
}

shared_ptr<ASTNode> AssignSimpleSyntaxRule::constructNode() {
    // check that initialized correctly
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // generate if needed
    if (!this->generated) {
        this->childRules = this->generateChildRules();
    }

    // create current node
    Token assignToken = Token::createEqualsToken();
    shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();


    shared_ptr<ASTNode> leftHandSide = this->childRules[LHS]->constructNode();
    shared_ptr<ASTNode> rightHandSide = this->childRules[RHS]->constructNode();

    assignNode->addChild(leftHandSide);
    assignNode->addChild(rightHandSide);

    return assignNode;
}
