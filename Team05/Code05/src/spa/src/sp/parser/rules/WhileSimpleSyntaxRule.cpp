#include <sp/parser/rules/WhileSimpleSyntaxRule.h>

const int WHILE_CONDITION_RULE = 0;
const int STMT_LIST_RULE = 1;

vector<shared_ptr<SimpleSyntaxRule>> WhileSimpleSyntaxRule::generateChildRules()
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

	// Statement rule
	shared_ptr<SimpleSyntaxRule> stmtListRule(new StatementListSimpleSyntaxRule);
	tokens = stmtListRule->consumeTokens(tokens); // consume the tokens
	childRules.push_back(stmtListRule);

	return childRules;
}

list<Token> WhileSimpleSyntaxRule::consumeTokens(list<Token> tokens)
{
	list<Token> childTokens;

	// Sanity check that the first token is a the while token
	Token token = tokens.front();

	if (!token.isWhileKeywordToken()) {
		throw SimpleSyntaxParserException("No while token found in WhileSimpleSyntaxRule");
	}
	tokens.pop_front();

	// Get condition tokens

	// First token should be open brackets
	token = tokens.front();
	if (!token.isOpenBracketToken()) {
		throw SimpleSyntaxParserException("While condition should start with an open bracket");
	}
	tokens.pop_front();

	// get rest of the condition
	int numOpenBracket = 1;
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
				break;
			}
		}
		childTokens.push_back(token);
	}

	if (!seenCloseBracket) {
		throw SimpleSyntaxParserException("No matching closing bracket for condition in while");
	}

	// Second get statementlst
	
	// first token should be open bracket
	token = tokens.front();
	if (!token.isOpenCurlyBracketToken()) {
		throw SimpleSyntaxParserException(string("Expected first token to be open bracket, but was ") + token.getString());
	}

	// then we keep going until we hit }
	seenCloseBracket = false;
	int numCloseBracket = 0;
	while (!tokens.empty() && !seenCloseBracket) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop

		if (token.isOpenCurlyBracketToken()) {
			numCloseBracket += 1;
		}
		else if (token.isClosedCurlyBracketToken()) {
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

	// assign and do state management
	this->tokens = childTokens;
	this->initialized = true;

	return tokens;
}

shared_ptr<ASTNode> WhileSimpleSyntaxRule::constructNode()
{
	// check that initialized correctly 
	if (!this->initialized) {
		throw SimpleSyntaxParserException("Node is not initialized!");
	}

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}
	
	Token whileToken = Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD);

	shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
	shared_ptr<ASTNode> conditionNode = this->childRules[WHILE_CONDITION_RULE]->constructNode();
	shared_ptr<ASTNode> stmtListNode = this->childRules[STMT_LIST_RULE]->constructNode();

	whileNode->addChild(conditionNode);
	whileNode->addChild(stmtListNode);

	return whileNode;
}
