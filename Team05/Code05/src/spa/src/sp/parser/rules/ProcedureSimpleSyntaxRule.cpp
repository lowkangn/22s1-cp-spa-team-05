#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <list>
using namespace std;

vector<SimpleSyntaxRule> ProcedureSimpleSyntaxRule::generateChildRules() {

	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	// a procedure is defined by a statement list, so let's parse each statement, which we identify by a delimiter
	vector<SimpleSyntaxRule> childRules;
	list<Token> tokens = this->tokens;
	
	Token token;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		if (token.isReadKeywordToken()) { // is read statement

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
		else if (token.isNameToken() && tokens.front().isEqualToken()) { // assign statement
			
		}
		else { // unknown
			throw SimpleSyntaxParserException("Unknown statement type within a procedure!")
		}
	}
	// if gotten here, tokens are empty
	return tokens;


}

list<Token> ProcedureSimpleSyntaxRule::consumeTokens(list<Token> tokens) {

	// a procedure consists of the procedure keyword sandwiched by two {} tokens
	// first token should be procedure keyword
	Token token = tokens.front();
	if (!token.isProcedureKeywordToken()) {
		throw SimpleSyntaxParserException(string("Expected first token to be procedure keyword, but was ") + token.getString());
	}
	tokens.pop_front(); // is ok, let's pop the top token out

	// second token should be {
	token = tokens.front();
	if (!token.isOpenCurlyBracketToken()) {
		throw SimpleSyntaxParserException(string("Expected first token to be open bracket, but was ") + token.getString());
	}
	tokens.pop_front(); // is ok, let's pop the top token out

	// then we keep going until we hit a } 
	list<Token> consumedTokens;
	bool seenCloseBracket = false;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		consumedTokens.push_back(token); // insert

		if (token.isClosedCurlyBracketToken()) {
			seenCloseBracket = true;
			break;
		}
	}
	// if no }, throw exception
	if (!seenCloseBracket) {
		throw SimpleSyntaxParserException("Missing closed bracket!")
	}
	// else, return remaining tokens and assign
	this->tokens = consumedTokens;
	this->initialized = true;

	return tokens;
}
