#pragma once

#include <assert.h>
#include <list>
#include <unordered_map>
#include <stack>

#include <qps/exceptions/PQLError.h>
#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query/clause/ClauseArgument.h>

using namespace std;

/* ClauseParser is an abstract class for the parsing of 
   all clauses. It contains default methods that are common
   in the parsing process across clauses and provides a template 
   method for parsing.
*/
class ClauseParser {
protected:
	list<PQLToken> tokens;

    /* Boolean for checking that getRemainingTokens is not called before parse */
    bool isParseCompleted = false;

	/* A mapping from each synonym to its type */
	unordered_map<string, ArgumentType> declarations;

	/* ==== Methods to parse a single argument of the clause and consume the token(s) used. ==== */

	ClauseArgument parseOneArgument();

	ClauseArgument parseSynonym();

	ClauseArgument parseStringLiteralOrPatternString();

	ClauseArgument parseStatementNumber();

	ClauseArgument parseWildcardOrPatternStringWithWildcards();

	/* ==== Methods ensure a delimiter is present and consume it ==== */
	
	/* Consumes the open bracket token. */
	void consumeOpenBracket();

	/* Consumes the comma token. */
	void consumeComma();

	/* Consumes the close bracket token. */
	void consumeCloseBracket();

	/*
		Helper function to determine precedence of operator
	*/
	// refer to https://www.tutorialspoint.com/Convert-Infix-to-Postfix-Expression#:~:text=To%20convert%20infix%20expression%20to,maintaining%20the%20precedence%20of%20them.
	int precedence(char c) {
		if (c == '/' || c == '*' || c == '%') {
			return 2;
		}
		else if (c == '+' || c == '-') {
			return 1;
		}
		else {
			return 0;
		}
	}

	// Stolen from Aryan, thank you Aryan <3
	string convertInfixToPostFix(string infixString) {
		stack<char> st; // to store the ordering of the operators

		string postFixString;

		string::iterator it;

		for (it = infixString.begin(); it != infixString.end(); it++) {
			char c = *it;
			if (isalnum(c)) {
				postFixString += c;
			}
			else if (c == '(') {
				st.push(c);
			}
			else if (c == ')') { // get all the operands inside the brackets
				while (!st.empty() && st.top() != '(') {
					postFixString += st.top();
					st.pop();
				}
				st.pop(); // remove the ')'
			}
			else {
				while (!st.empty() && this->precedence(c) <= this->precedence(st.top())) {
					postFixString += st.top();
					st.pop();
				}
				st.push(c);
			}
		}

		while (!st.empty()) {
			postFixString += st.top();
			st.pop();
		}
		return postFixString;
	};

public:

	ClauseParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) {
		this->tokens = tokens;
		this->declarations = declarations;
	}

	/* Returns the tokens stored in this ClauseParser */
	list<PQLToken> getRemainingTokens();
};
