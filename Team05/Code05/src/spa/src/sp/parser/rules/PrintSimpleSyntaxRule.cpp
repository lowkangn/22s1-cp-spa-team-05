#include <sp/parser/rules/PrintSimpleSyntaxRule.h>

vector<shared_ptr<SimpleSyntaxRule>> PrintSimpleSyntaxRule::generateChildRules()
{
	vector<shared_ptr<SimpleSyntaxRule>> childRules;
	// only child is variable
	shared_ptr<SimpleSyntaxRule> rulePointer = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
	rulePointer->consumeTokens(this->tokens);
	childRules.push_back(rulePointer);
	return childRules;

}

list<Token> PrintSimpleSyntaxRule::consumeTokens(list<Token> tokens)
{
	// a print statement consists of a print token, a variable name token that is not a reserved word 
	// and a semicolon
	list<Token> childTokens;

	// print token
	Token token = tokens.front(); // read
	tokens.pop_front(); // pop
	if (!token.isPrintKeywordToken()) {
		throw SimpleSyntaxParserException("first token should be a print token.");
	}

	// variable token
	token = tokens.front();
	tokens.pop_front(); // pop
	if (!token.isNameToken()) {
		throw SimpleSyntaxParserException("second token shouold be a name variable token.");
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

shared_ptr<ASTNode> PrintSimpleSyntaxRule::constructNode() {
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// create print node
	Token readToken = Token::createPrintToken();
	shared_ptr<ASTNode> printNode(new PrintASTNode(readToken));

	// create child variable node
	shared_ptr<ASTNode> variableNode = this->childRules.front()->constructNode();

	printNode->addChild(variableNode);

	return printNode;
}