#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>

vector<shared_ptr<SimpleSyntaxRule>> ProgramSimpleSyntaxRule::generateChildRules() {
    /*
        By definition, a program must be made up of multiple procedures. 
        So with the tokens at hand, we need to look out for procedure keywords/symbols 
        and group them into corresponding child procedure rules.
    */

    vector<shared_ptr<SimpleSyntaxRule>> childRules; // create arraylist
    list<Token> tokens = this->tokens;
    list<Token> remainingTokens;
    while (!tokens.empty()) {
        // create child rule and let it consume the tokens it needs
        shared_ptr<SimpleSyntaxRule> childProcedurePointer = make_shared<
            ProcedureSimpleSyntaxRule>();
        remainingTokens = childProcedurePointer->consumeTokens(tokens);

        // check that it actually consumed some tokens
        if ((remainingTokens.size() == tokens.size()) && (tokens.size() != 0)) {
            // none consumed; this should not happen,
            // since a program must be made up of an exact number of procedures
            throw SimpleSyntaxParserException(
                "Unable to parse another procedure when there are still tokens left. This should not happen!"); //NOLINT(whitespace/line_length)
        }

        // all is ok, add this rule to list of rules
        childRules.push_back(childProcedurePointer);

        // set remaining to tokens
        tokens = remainingTokens;
    }

    return childRules;
}

list<Token> ProgramSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
    // a program is the root, and should consume all tokens
    // assign and do state management
    this->tokens = tokens;
    this->initialized = true;

    // then return an empty list of tokens
    return list<Token>();
}

shared_ptr<ASTNode> ProgramSimpleSyntaxRule::constructNode() {
    // check that initialized correctly
    if (!this->initialized) {
        throw SimpleSyntaxParserException("Node is not initialized!");
    }

    // generate if needed
    if (!this->generated) {
        this->childRules = this->generateChildRules();
    }

    // create current node
    Token programToken = Token::createProgramToken();
    shared_ptr<ASTNode> node = ProgramASTNode::createProgramNode();
    // for each rule, recursively create children and assign to children
    for (auto rulePointer = this->childRules.begin(); rulePointer != this->childRules.end();
         rulePointer++) {
        shared_ptr<ASTNode> child = (*rulePointer)->constructNode();
        node->addChild(child);
    }

    return node;
}

int ProgramSimpleSyntaxRule::setASTLineNumbers(shared_ptr<ASTNode> root, int lineNumber) {
    if (root->isStmtLstNode()) {
        // Node is a statement list: call function to handle statement list
        lineNumber = this->setStatementListLineNumber(root, lineNumber);
    } else if (root->isProgramNode() || root->isProcedureNode()) {
        // Node is a program or procedure
        // Iterate through children and recursively set their line numbers
        for (shared_ptr<ASTNode> child : root->getChildren()) {
            lineNumber = this->setASTLineNumbers(child, lineNumber);
        }
    }

    return lineNumber;
}

int ProgramSimpleSyntaxRule::setWhileStatementLineNumber(
    shared_ptr<ASTNode> root, int lineNumber) {
    shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(root);
    this->recursiveSetStatementNumber(root, lineNumber);
    this->recursiveSetStatementNumber(whileNode->getCondition(), lineNumber);
    lineNumber += 1;

    lineNumber = this->setStatementListLineNumber(whileNode->getStmtList(), lineNumber);

    return lineNumber;
}

int ProgramSimpleSyntaxRule::setIfStatementLineNumber(shared_ptr<ASTNode> root, int lineNumber) {
    shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(root);
    this->recursiveSetStatementNumber(root, lineNumber);
    this->recursiveSetStatementNumber(ifNode->getCondition(), lineNumber);
    lineNumber += 1;

    lineNumber = this->setStatementListLineNumber(ifNode->getThenStatements(), lineNumber);
    lineNumber = this->setStatementListLineNumber(ifNode->getElseStatements(), lineNumber);

    return lineNumber;
}

void ProgramSimpleSyntaxRule::recursiveSetStatementNumber(
    shared_ptr<ASTNode> root, int lineNumber) {
    if (!root->isStmtLstNode()) {
        root->setLineNumber(lineNumber);
        // We only want children which are not If and While nodes as they contain statementlists
        if (root->numChildren() > 0 && !root->isIfNode() && !root->isWhileNode()) {
            // Set children of the tree to the same line number recursively
            for (shared_ptr<ASTNode> child : root->getChildren()) {
                this->recursiveSetStatementNumber(child, lineNumber);
            }
        }
    } else {
        throw SimpleSyntaxParserException(
            "Statement list encountered in recursiveSetStatementNumber");
    }
}

int ProgramSimpleSyntaxRule::setStatementListLineNumber(shared_ptr<ASTNode> root, int lineNumber) {
    // For each statement set the line number then increment
    for (shared_ptr<ASTNode> child : root->getChildren()) {
        if (child->isIfNode()) {
            lineNumber = this->setIfStatementLineNumber(child, lineNumber);
        } else if (child->isWhileNode()) {
            lineNumber = this->setWhileStatementLineNumber(child, lineNumber);
        } else if (child->isCallNode()) {
            child->setLineNumber(lineNumber);
            lineNumber += 1;
        } else {
            // All other statements
            this->recursiveSetStatementNumber(child, lineNumber);
            lineNumber += 1;
        }
    }

    return lineNumber;
}
