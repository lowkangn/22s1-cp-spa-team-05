#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/PatternClause.h>
#include <common/PostFixConverter.h>

#include <iostream>

const string STRING_DELIMITER = " ";

/* PatternParser is an abstract class for the parsing of pattern
 * clauses. It implements the extraction of their arguments.
 */
class PatternParser : public ClauseParser {
protected:
	ClauseArgument parsePatternString() {
		// Ignore '"' token
		this->tokens.pop_front();

		// Add token strings to pattern string until quote
		string s;
		bool first = true;
		while(!this->tokens.front().isQuote()) {
			// Must be name/integer followed by operator, until last name/integer

			// Must be name/integer
			PQLToken currentToken = this->tokens.front();
			if (!currentToken.isName() && !currentToken.isInteger()) {
				throw PQLError("Invalid syntax for pattern string");
			}
			s += currentToken.getTokenString();
			this->tokens.pop_front();

			// If end of string, stop
			if (this->tokens.front().isQuote()) {
				break;
			}
			else {
				// else, pad with delimiter
				s += STRING_DELIMITER;
			}

			// Must be operator
			currentToken = this->tokens.front();
			if (!this->tokens.front().isOperator()) {
				throw PQLError("Invalid syntax for pattern string");
			}
			s += currentToken.getTokenString() + STRING_DELIMITER; // pad with delimiter
			this->tokens.pop_front();
		}

		// Check quote
		if(this->tokens.empty() || !this->tokens.front().isQuote()) {
			throw PQLError("Expected closing quote");
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
			throw PQLError("Expected closing underscore");
		}
		this->tokens.pop_front();

		string s = "_";
		s += patternString.getIdentifier();
		s += "_";
		return ClauseArgument::createPatternStringWithWildcardsArg(s);
	}

public:
	PatternParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
			ClauseParser(tokens, declarations) {};

	/* Parses this parser's tokens into a Clause. (Template method:
	   behaviour depends on implementation of virtual functions) */
	shared_ptr<PatternClause> parse();

	/* Extracts the LHS and RHS arguments of the such that clause */
	virtual vector<ClauseArgument> extractArguments() = 0;

	/* Returns true if the clauseTypeToken matches the clause that the Parser parses */
	virtual bool isCorrectClauseType(PQLToken clauseTypeToken) = 0;

	/* Checks that the arguments fulfill the constraints of the clause; throws a PQL error otherwise */
	virtual void checkArguments(vector<ClauseArgument>& args) = 0;

	/* Creates the clause given the clause type and arguments */
	virtual shared_ptr<PatternClause> createClause(PQLToken clauseTypeToken, vector<ClauseArgument>& args) = 0;
};