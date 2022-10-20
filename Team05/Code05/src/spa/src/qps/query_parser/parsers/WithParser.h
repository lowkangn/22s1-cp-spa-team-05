#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/AttributeClauseParser.h>
#include <qps/query/clause/WithBothExactClause.h>
#include <qps/query/clause/WithOneExactClause.h>
#include <qps/query/clause/WithNoExactClause.h>

#include <iostream>

/* WithParser is a class for the parsing of the with clause.
 */
class WithParser : public AttributeClauseParser {
protected:

	/* Extracts the ref portion of an attrCompare */
	vector<ClauseArgument> parseRef();

	/* Creates the WithClause from the extracted ClauseArguments */
	shared_ptr<WithClause> createWithClause(vector<ClauseArgument>& lhsArgs, vector<ClauseArgument>& rhsArgs);

	/* Checks for an equals and consumes it */
	void consumeEquals();

	/* Checks the semantics of one ref. */
	void checkRef(vector<ClauseArgument>& args);

	/* Checks the semantics of the attrCompare. */
	void checkAttrCompare(vector<ClauseArgument>& lhsArgs, vector<ClauseArgument>& rhsArgs);

public:
	WithParser(list<PQLToken>& tokens, unordered_map<string, ArgumentType> declarations) :
		AttributeClauseParser(tokens, declarations) {};

	/* Parses this with parser's tokens into a with clause. */
	shared_ptr<WithClause> parse();
};