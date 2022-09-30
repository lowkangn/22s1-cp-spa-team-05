#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>

list<Token> StatementListSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// a statement list lies between { and }, so we consume only the middle ones
	list<Token> childTokens;
	
	// first token should be open bracket
	Token token = tokens.front();
	if (!token.isOpenCurlyBracketToken()) {
		throw SimpleSyntaxParserException(string("StatementList: Expected first token to be open bracket, but was ") + token.getString());
	}
	tokens.pop_front();

	// then we keep going until we hit }
	bool seenCloseBracket = false;
	bool seenOneToken = false;
	int numOpenBracketSeen = 1;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		if (token.isOpenCurlyBracketToken()) {
			numOpenBracketSeen += 1;
		} else if (token.isClosedCurlyBracketToken()) {
			numOpenBracketSeen -= 1;
			if (numOpenBracketSeen == 0) {
				seenCloseBracket = true;
				break;
			}
		}
		else {
			seenOneToken = true;
		}
		childTokens.push_back(token); // insert all tokens in order within bracket
	}
	// if no }, throw exception
	if (!seenCloseBracket) {
		throw SimpleSyntaxParserException("Missing closed bracket!");
	}

	// If not tokens in the statement list return error
	if (!seenOneToken) {
		throw SimpleSyntaxParserException("No tokens in statement list");
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
		// Sanity check
		if (!(tokens.size() >= 2)) {
			throw SimpleSyntaxParserException("Less than one token found while trying to parse for a statement in statement list");
		}
		else if (this->isReadStatement(tokens)) { // is read statement
			shared_ptr<SimpleSyntaxRule> assignRulePointer = shared_ptr<SimpleSyntaxRule>(new ReadSimpleSyntaxRule());
			tokens = assignRulePointer->consumeTokens(tokens);
			childRules.push_back(assignRulePointer); // add to children nodes in order
		}
		else if (this->isCallStatement(tokens)) { // call statement
			// TODO - not needed for Milestone 1
		}
		else if (this->isWhileStatement(tokens)) { // while block
			shared_ptr<SimpleSyntaxRule> whileRulePointer = shared_ptr<SimpleSyntaxRule>(new WhileSimpleSyntaxRule());
			tokens = whileRulePointer->consumeTokens(tokens); // consume tokens
			childRules.push_back(whileRulePointer); // add to children nodes in order
		}
		else if (this->isIfStatement(tokens)) { // if else statement
			shared_ptr<SimpleSyntaxRule> ifRulePointer = shared_ptr<SimpleSyntaxRule>(new IfSimpleSyntaxRule());
			tokens = ifRulePointer->consumeTokens(tokens); // consume tokens
			childRules.push_back(ifRulePointer); // add to children nodes in order
		}
		else if (this->isPrintStatement(tokens)) {
			shared_ptr<SimpleSyntaxRule> printRulePointer = shared_ptr<SimpleSyntaxRule>(new PrintSimpleSyntaxRule());
			tokens = printRulePointer->consumeTokens(tokens); // consume tokens
			childRules.push_back(printRulePointer); // add to children nodes in order
		}
		else if (this->isAssignStatement(tokens)) {
			shared_ptr<SimpleSyntaxRule> readRulePointer = shared_ptr<SimpleSyntaxRule>(new AssignSimpleSyntaxRule());
			tokens = readRulePointer->consumeTokens(tokens); // consume tokens
			childRules.push_back(readRulePointer); // add to children nodes in order
		}
		else { // unknown
			throw SimpleSyntaxParserException("Unknown token when trying to parse statementlist!");
		}
	}
	// if gotten here, tokens are empty
	return childRules;
}

shared_ptr<ASTNode> StatementListSimpleSyntaxRule::constructNode() {
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
	Token stmtLstToken = Token::createPlaceholderToken();
	shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
	// for each rule, recursively create children and assign to stmtLst node
	for (auto rulePointer = this->childRules.begin(); rulePointer != this->childRules.end(); rulePointer++) {
		shared_ptr<ASTNode> child = (*rulePointer)->constructNode();
		stmtLstNode->addChild(child);
	}

	return stmtLstNode;
}

bool StatementListSimpleSyntaxRule::isAssignStatement(list<Token> tokens) {
	// Create Iterators
	list<Token>::iterator it = tokens.begin();

	Token firstElement = *it;
	Token SecondElement = *(next(it));

	// If first element is While
	if (firstElement.isNameToken() && SecondElement.isEqualToken()) {
		return true;
	}

	return false;
}

bool StatementListSimpleSyntaxRule::isWhileStatement(list<Token> tokens) {
	// Create Iterator
	list<Token>::iterator it = tokens.begin();

	Token firstElement = *it;
	Token SecondElement = *(next(it));

	// If first element is While
	if (firstElement.isWhileKeywordToken() && SecondElement.isOpenBracketToken()) {
		return true;
	}

	return false;
}

bool StatementListSimpleSyntaxRule::isIfStatement(list<Token> tokens) {
	// Create Iterator
	list<Token>::iterator it = tokens.begin();

	Token firstElement = *it;
	Token SecondElement = *(next(it));

	// If first element is While
	if (firstElement.isIfKeywordToken() && SecondElement.isOpenBracketToken()) {
		return true;
	}

	return false;
}

bool StatementListSimpleSyntaxRule::isPrintStatement(list<Token> tokens) {
	// Create Iterator
	list<Token>::iterator it = tokens.begin();

	Token firstElement = *it;
	Token SecondElement = *(next(it));

	// If first element is While
	if (firstElement.isPrintKeywordToken() && SecondElement.isNameToken()) {
		return true;
	}

	return false;
}

bool StatementListSimpleSyntaxRule::isReadStatement(list<Token> tokens) {
	// Create Iterator
	list<Token>::iterator it = tokens.begin();

	Token firstElement = *it;
	Token SecondElement = *(next(it));

	// If first element is While
	if (firstElement.isReadKeywordToken() && SecondElement.isNameToken()) {
		return true;
	}

	return false;
}

bool StatementListSimpleSyntaxRule::isCallStatement(list<Token> tokens) {
	// Create Iterator
	list<Token>::iterator it = tokens.begin();

	Token firstElement = *it;
	Token SecondElement = *(next(it));

	// If first element is While
	if (firstElement.isCallKeywordToken() && SecondElement.isNameToken()) {
		return true;
	}

	return false;
}
