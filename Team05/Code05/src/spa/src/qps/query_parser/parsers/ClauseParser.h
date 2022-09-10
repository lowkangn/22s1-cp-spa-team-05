#pragma once

#include <assert.h>
#include <list>
#include <unordered_map>

#include <qps/exceptions/PQLError.h>
#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query/clause/Clause.h>
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

	/* A mapping from each synonym to its type */
	unordered_map<string, ArgumentType> declarations;
	
	// ==== Methods to parse a single argument of the clause and consume the token(s) used. ====

	ClauseArgument parseOneArgument();

	ClauseArgument parseSynonym();

	ClauseArgument parseStringLiteral();

	ClauseArgument parseStatementNumber();

	ClauseArgument parseWildcard();

	// ==== Methods ensure a delimiter is present and consume it ==== 
	
	/* Consumes the open bracket token. */
	void consumeOpenBracket();

	/* Consumes the comma token. */
	void consumeComma();

	/* Consumes the close bracket token. */
	void consumeCloseBracket();

public:

	ClauseParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) {
		this->tokens = tokens;
		this->declarations = declarations;
	}

	/* Returns the tokens stored in this ClauseParser */
	list<PQLToken> getRemainingTokens();

	/* Parses this parser's tokens into a Clause. (Template method:
	   behaviour depends on implementation of virtual functions) */
	shared_ptr<Clause> parse();

	/* Returns true if the clauseTypeToken matches the clause that the Parser parses */
	virtual bool isCorrectClauseType(PQLToken clauseTypeToken) = 0;

	/* Extracts the arguments of the clause. Consumes all tokens in the clause after 
	   'Select', relRef or 'Pattern' */
	virtual list<ClauseArgument> extractArguments() = 0;

	/* Checks that the arguments fulfill the constraints of the clause; throws a PQL error otherwise */
	virtual void checkArguments(list<ClauseArgument>& args) = 0;

	/* Creates the clause given the clause type and arguments */
	virtual shared_ptr<Clause> createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) = 0;
};
