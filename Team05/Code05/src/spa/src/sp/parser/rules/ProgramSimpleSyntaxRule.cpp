#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>
#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>

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
		shared_ptr<SimpleSyntaxRule> childProcedurePointer = shared_ptr<SimpleSyntaxRule>(new ProcedureSimpleSyntaxRule());
		remainingTokens = childProcedurePointer->consumeTokens(tokens);

		// check that it actually consumed some tokens
		if ((remainingTokens.size() == tokens.size()) && (tokens.size()!=0)) {
			// none consumed
			// this should not happen, since the a program must be made up of an exact number of procedures
			throw SimpleSyntaxParserException(
				"Unable to parse another procedure when there are still tokens left. This should not happen!"
			);
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
	Token programToken = Token{ PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD };
	shared_ptr<ASTNode> node (new ProgramASTNode(programToken));

	// for each rule, recursively create children and assign to children
	for (auto rulePointer = this->childRules.begin(); rulePointer != this->childRules.end(); rulePointer++) {
		shared_ptr<ASTNode> child = (*rulePointer)->constructNode();
		node->addChild(child);
	}

	return node;
}

int ProgramSimpleSyntaxRule::setASTLineNumbers(shared_ptr<ASTNode> root, int lineNumber) {
	ASTNodeType nodeType = root->getType();

	// If it is a statement list call function to handle statement list
	if (nodeType == ASTNodeType::STMTLIST) {
		lineNumber = this->iterateThroughStatementInStatementList(root, lineNumber);
	}

	// Iterate through children and recursively set their line numbers
	for (auto &child : root->getChildren()) {
		lineNumber = this->setASTLineNumbers(child, lineNumber);
	}

	return lineNumber;
}

int ProgramSimpleSyntaxRule::iterateThroughStatementInStatementList(shared_ptr<ASTNode> rootOfStatementList , int lineNumber) {
	// Sanity check
	assert(rootOfStatementList->getType() == ASTNodeType::STMTLIST);

	// For each statement set the line number then increment
	for (auto &child : rootOfStatementList->getChildren()) {
		this->recursiveSetLineNumber(child, lineNumber);
		lineNumber += 1;
	}
	return lineNumber;
}

void ProgramSimpleSyntaxRule::recursiveSetLineNumber(shared_ptr<ASTNode> root, int lineNumber) {
	// Set current node's line number
	ASTNodeType nodeType = root->getType();
	root->setLineNumber(lineNumber);

	// TODO Try to come up with a more clean way to handle this logic
	switch (nodeType) {
	// If and While are special cases
	case ASTNodeType::IF:
	case ASTNodeType::WHILE:
		{
			for (auto &child : root->getChildren()) {
				// Call to iterateThroughStatementInStatementList as IF and WHILE themselves contain statement lists
				if (child->getType() == ASTNodeType::STMTLIST) {
					lineNumber = this->iterateThroughStatementInStatementList(child, lineNumber);
				}
				else {
					// Handles the condition in IF statements
					this->recursiveSetStatementNumber(root, lineNumber);
				}
			}
			break;
		}
	default:
		{
			// Set the entire subtree to the same line number
			this->recursiveSetStatementNumber(root, lineNumber);
		}
	}
}

void ProgramSimpleSyntaxRule::recursiveSetStatementNumber(shared_ptr<ASTNode> root, int lineNumber) {
	root->setLineNumber(lineNumber);
	if (root->numChildren() > 0) {
		// Set children of the tree to the same line number recursively
		for (auto& child : root->getChildren()) {
			this->recursiveSetStatementNumber(child, lineNumber);
		}
	}
}
