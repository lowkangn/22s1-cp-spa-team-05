#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/PatternClause.h>
#include <common/PostFixConverter.h>
#include <qps/query/clause/PatternAssignClause.h>
#include <qps/query/clause/PatternIfClause.h>
#include <qps/query/clause/PatternWhileClause.h>


#include <iostream>

const string STRING_DELIMITER = " ";

/* PatternParser is an abstract class for the parsing of pattern
 * clauses. It implements the extraction of their arguments.
 */
class PatternParser : public ClauseParser {
protected:
	ClauseArgument parsePatternString() {
		assert(this->tokens.front().isQuote());
		// Ignore '"' token
		this->tokens.pop_front();

		// Add token strings to pattern string until quote
		string s;

		int numBrackets = 0;
		bool isPreviousFactor = false; // factor : NAME | INTEGER | '(' expr ')'

		while(!this->tokens.empty() && !this->tokens.front().isQuote()) {
			PQLToken currentToken = this->tokens.front();

			/* Open brackets and factors can appear anywhere except immediately after a factor
			   Operators and close brackets can only appear after a factor */
			if (!isPreviousFactor && currentToken.isOpenBracket()) {
				numBrackets++;
				s += currentToken.getTokenString();
				this->consumeOpenBracket();
				isPreviousFactor = false;
			} 
			else if (isPreviousFactor && currentToken.isOperator()) {
				s += currentToken.getTokenString();
				this->tokens.pop_front();
				isPreviousFactor = false;
			}
			else if (isPreviousFactor && currentToken.isCloseBracket()) {
				numBrackets--;
				s += currentToken.getTokenString();
				this->consumeCloseBracket();
				isPreviousFactor = true;
			}
			else if (!isPreviousFactor && (currentToken.isName() || currentToken.isInteger())) {
				s += currentToken.getTokenString();
				this->tokens.pop_front();
				isPreviousFactor = true;
			} 
			else {
				throw PQLSyntaxError("Unexpected token in pattern string: " + currentToken.getTokenString());
			}

			// Pad if the pattern string has not ended
			if (!this->tokens.front().isQuote()) {
				s += STRING_DELIMITER;
			}

		}

		// Check balanced brackets
		if (numBrackets != 0) {
			throw PQLSyntaxError("Unmatched bracket in pattern string");
		}

		// Check quote
		if(this->tokens.empty() || !this->tokens.front().isQuote()) {
			throw PQLSyntaxError("Expected closing quote for pattern string");
		}
		this->tokens.pop_front();

		PostFixConverter converter = PostFixConverter();
		string postFixString = converter.convertInfixToPostFix(s);
		return ClauseArgument::createPatternStringArg(postFixString);
	}

	ClauseArgument parsePatternStringWithWildcards() {
		ClauseArgument patternString = parsePatternString();

		// Check '_'
		if(this->tokens.empty() || !this->tokens.front().isUnderscore()) {
			throw PQLSyntaxError("Expected closing underscore");
		}
		this->tokens.pop_front();

		string s = "_";
		s += patternString.getIdentifier();
		s += "_";
		return ClauseArgument::createPatternStringWithWildcardsArg(s);
	}

public:
	PatternParser(list<PQLToken>& tokens, unordered_map<string, ArgumentType> declarations) :
			ClauseParser(tokens, declarations) {};

	/* Parses this parser's tokens into a Clause. */
	shared_ptr<PatternClause> parse();

	/* Extracts the LHS and RHS arguments of the such that clause */
	vector<ClauseArgument> extractArguments();

	/* Checks that the arguments fulfill the constraints of the clause; throws a PQL error otherwise */
	void checkArguments(vector<ClauseArgument>& args);

	/* Creates the clause given the clause type and arguments */
	shared_ptr<PatternClause> createClause(PQLToken clauseTypeToken, vector<ClauseArgument>& args);
};