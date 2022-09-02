#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

#include <list>
using namespace std;

vector<shared_ptr<SimpleSyntaxRule>> ProcedureSimpleSyntaxRule::generateChildRules() {

	// must be initialized
	if (!this->initialized) {
		throw SimpleSyntaxRuleNotInitializedException();
	}

	// first token should be a procedure name

	// a procedure is defined by a statement list, so let's parse each statement, which we identify by a delimiter
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
			throw SimpleSyntaxParserException("Unknown statement type within a procedure!");
		}
	}
	// if gotten here, tokens are empty
	return childRules;

}

list<Token> ProcedureSimpleSyntaxRule::consumeTokens(list<Token> tokens) {

	list<Token> consumedTokens;

	// a procedure consists of the procedure keyword and name then statements sandwiched by two {} tokens
	// first token should be procedure keyword
	Token token = tokens.front();
	if (!token.isProcedureKeywordToken()) {
		throw SimpleSyntaxParserException(string("Expected first token to be procedure keyword, but was ") + token.getString());
	}
	tokens.pop_front(); // is ok, let's pop the top token out

	// second token should be a name
	if (!token.isNameToken()) {
		throw SimpleSyntaxParserException(string("Expected second token to be valid name, but was ") + token.getString());
	}
	token = tokens.front(); // read
	tokens.pop_front(); // pop
	consumedTokens.push_back(token);

	// third token should be {
	token = tokens.front();
	if (!token.isOpenCurlyBracketToken()) {
		throw SimpleSyntaxParserException(string("Expected third token to be open bracket, but was ") + token.getString());
	}
	tokens.pop_front(); // is ok, let's pop the top token out

	// then we keep going until we hit a } 
	bool seenCloseBracket = false;
	while (!tokens.empty()) {
		token = tokens.front(); // read
		tokens.pop_front(); // pop
		consumedTokens.push_back(token); // insert all tokens in order within bracket

		if (token.isClosedCurlyBracketToken()) {
			seenCloseBracket = true;
			break;
		}
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

ASTNode ProcedureSimpleSyntaxRule::constructNode() {
	// TODO
	return ASTNode();
}