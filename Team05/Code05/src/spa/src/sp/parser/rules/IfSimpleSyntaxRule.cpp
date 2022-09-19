#include <sp/parser/rules/IfSimpleSyntaxRule.h>

const int IF_CONDITION_RULE = 0;
const int THEN_RULE = 1;
const int ELSE_RULE = 2;

bool isSafeToPop(list<Token> tokens) {
	if (tokens.size() == 0) {
		return false;
	}
	return true;
}

vector<shared_ptr<SimpleSyntaxRule>> IfSimpleSyntaxRule::generateChildRules()
{
	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	vector<shared_ptr<SimpleSyntaxRule>> childRules;
	list<Token> tokens = this->tokens;

	// Conditional Rule
	shared_ptr<SimpleSyntaxRule> conditionRule(new ConditionalExpressionSimpleSyntaxRule);
	tokens = conditionRule->consumeTokens(tokens); // consume the tokens
	childRules.push_back(conditionRule);

	// Then Statement rule
	shared_ptr<SimpleSyntaxRule> thenStmtListRule(new StatementListSimpleSyntaxRule);
	tokens = thenStmtListRule->consumeTokens(tokens); // consume the tokens
	childRules.push_back(thenStmtListRule);

	// Else Statement rule
	shared_ptr<SimpleSyntaxRule> elseStmtListRule(new StatementListSimpleSyntaxRule);
	tokens = elseStmtListRule->consumeTokens(tokens); // consume the tokens
	childRules.push_back(elseStmtListRule);

	return childRules;
}

list<Token> IfSimpleSyntaxRule::consumeTokens(list<Token> tokens)
{
	list<Token> childTokens;

	// Sanity check that the first token is a the if token
	Token token = tokens.front();

	if (!token.isIfKeywordToken()) {
		throw SimpleSyntaxParserException("No If token found in IfSimpleSyntaxRule");
	}
	tokens.pop_front();

	// Get condition tokens

	// First token should be open brackets
	if (!isSafeToPop(tokens) || !tokens.front().isOpenBracketToken()) {
		throw SimpleSyntaxParserException("If condition should start with an open bracket");
	}

	// get rest of the condition
	int numOpenBracket = 0;
	bool seenCloseBracket = false;

	while (!tokens.empty() && !seenCloseBracket) {
		token = tokens.front();
		tokens.pop_front();

		if (token.isOpenBracketToken()) {
			numOpenBracket += 1;
		}
		else if (token.isClosedBracketToken()) {
			numOpenBracket -= 1;
			if (numOpenBracket == 0) {
				seenCloseBracket = true;
			}
		}
		childTokens.push_back(token);
	}

	// get then token and stmt list
	
	// Next token should be then

	if (!isSafeToPop(tokens) || !tokens.front().isThenKeywordToken()) {
		throw SimpleSyntaxParserException("No then token found in IfSimpleSyntaxRule");
	}
	tokens.pop_front();

	list<Token> thenTokens = getStmtList(tokens);
	childTokens.insert(childTokens.end(), thenTokens.begin(), thenTokens.end());

	// get else token and stmt list

	// Next token should be else
	if (!isSafeToPop(tokens) || !tokens.front().isElseKeywordToken()) {
		throw SimpleSyntaxParserException("No else token found in IfSimpleSyntaxRule");
	}
	tokens.pop_front();

	list<Token> elseTokens = getStmtList(tokens);
	childTokens.insert(childTokens.end(), elseTokens.begin(), elseTokens.end());

	// assign and do state management
	this->tokens = childTokens;
	this->initialized = true;

	return tokens;
}

shared_ptr<ASTNode> IfSimpleSyntaxRule::constructNode()
{
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	Token ifToken = Token::createIfToken();

	shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));
	shared_ptr<ASTNode> conditionNode = this->childRules[IF_CONDITION_RULE]->constructNode();
	shared_ptr<ASTNode> thenNode = this->childRules[THEN_RULE]->constructNode();
	shared_ptr<ASTNode> elseNode = this->childRules[ELSE_RULE]->constructNode();

	ifNode->addChild(conditionNode);
	ifNode->addChild(thenNode);
	ifNode->addChild(elseNode);

	return ifNode;
}

list<Token> IfSimpleSyntaxRule::getStmtList(list<Token>& tokens)
{
	list<Token> childTokens;

	// first token should be open bracket
	Token token = tokens.front();
	if (!token.isOpenCurlyBracketToken()) {
		throw SimpleSyntaxParserException(string("Expected first token to be open curly bracket, but was ") + token.getString());
	}

	// then we keep going until we hit }
	bool seenCloseBracket = false;
	int numCloseBracket = 0;
	while (!tokens.empty() && !seenCloseBracket) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop

		if (token.isOpenCurlyBracketToken()) {
			numCloseBracket += 1;
		} else if (token.isClosedCurlyBracketToken()) {
			numCloseBracket -= 1;
			if (numCloseBracket == 0) {
				seenCloseBracket = true;
			}
		}
		childTokens.push_back(token); // insert all tokens in order within bracket
	}
	// if no }, throw exception
	if (!seenCloseBracket) {
		throw SimpleSyntaxParserException("Missing closed bracket!");
	}

	return childTokens;
}
