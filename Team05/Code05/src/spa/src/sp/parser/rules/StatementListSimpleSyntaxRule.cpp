#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/tokens/Token.h>
#include <list>
using namespace std;

list<Token> StatementListSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// a statement list lies between { and }, so we consume only the middle ones
	list<Token> childTokens;
	
	// first token should be open bracket
	Token token = tokens.front();
	if (!token.isOpenCurlyBracketToken()) {
		throw SimpleSyntaxParserException(string("Expected first token to be open bracket, but was ") + token.getString());
	}
	tokens.pop_front();

	// then we keep going until we hit }
	bool seenCloseBracket = false;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		
		if (token.isClosedCurlyBracketToken()) {
			seenCloseBracket = true;
			break;
		}
		childTokens.push_back(token); // insert all tokens in order within bracket
	}
	// if no }, throw exception
	if (!seenCloseBracket) {
		throw SimpleSyntaxParserException("Missing closed bracket!");
	}

	// assign and do state management
	this->tokens = childTokens;
	this->initialized = true;

	return tokens;
}

vector<shared_ptr<SimpleSyntaxRule>> StatementListSimpleSyntaxRule::generateChildRules() {
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	// parse block
	vector<shared_ptr<SimpleSyntaxRule>> childRules;
	list<Token> tokens = this->tokens;
	while (!tokens.empty()) {
		Token token = tokens.front(); // read
		if (token.isReadKeywordToken()) { // is read statement
			shared_ptr<SimpleSyntaxRule> readRulePointer = shared_ptr<SimpleSyntaxRule>(new ReadSimpleSyntaxRule());
			tokens = readRulePointer->consumeTokens(tokens); // consume tokens
			childRules.push_back(readRulePointer); // add to children nodes in order
		}
		else if (token.isCallKeywordToken()) { // call statement
			// TODO - not needed for MVP
		}
		else if (token.isWhileKeywordToken()) { // while block
			// TODO - not needed for MVP
		}
		else if (token.isIfKeywordToken()) { // if else statement
			// TODO - not needed for MVP
		}
		else if (token.isNameToken()) { // probably assign statement
			shared_ptr<SimpleSyntaxRule> assignRulePointer = shared_ptr<SimpleSyntaxRule>(new AssignSimpleSyntaxRule());
			tokens = assignRulePointer->consumeTokens(tokens);
			childRules.push_back(assignRulePointer); // add to children nodes in order
		}
		else { // unknown
			throw SimpleSyntaxParserException("Unknown token when trying to parse statementlist!");
		}
	}
	// if gotten here, tokens are empty
	return childRules;
}

shared_ptr<ASTNode> StatementListSimpleSyntaxRule::constructNode() {
	// TODO
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// create stmt node
	// Add stmtLst as a token type
	Token stmtLstToken = Token{ "", TokenType::DELIMITER};
	shared_ptr<ASTNode> stmtLstNode(new ASTNode(vector<Token>{stmtLstToken}));
	stmtLstNode->setType(ASTNodeType::STMTLIST);

	// for each rule, recursively create children and assign to stmtLst node
	for (auto rulePointer = this->childRules.begin(); rulePointer != this->childRules.end(); rulePointer++) {
		shared_ptr<ASTNode> child = (*rulePointer)->constructNode();
		stmtLstNode->addChild(child);
	}

	return stmtLstNode;
}