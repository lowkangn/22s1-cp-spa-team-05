#pragma once

#include <list>
#include <unordered_map>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query/clause/WithClause.h>

#include <iostream>

/* WithParser is a class for the parsing of the with clause.
 */
class WithParser : public ClauseParser {
protected:

	/* Extracts the ref portion of an attrCompare */
	vector<ClauseArgument> parseRef();

	/* Parses the attrName portion of an attrRef */
	ClauseArgument parseAttribute();

	/* Checks for a dot and consumes it */
	void consumeDot();

	/* Checks for an equals and consumes it */
	void consumeEquals();

	/* Checks the semantics of one ref; throws a PQLSemanticError if semantically invalid */
	void checkRef(vector<ClauseArgument>& args);

	/* Checks the semantics of the attrCompare; throws a PQLSemanticError if semantically invalid */
	void checkAttrCompare(vector<ClauseArgument>& lhsArgs, vector<ClauseArgument>& rhsArgs);

public:
	WithParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations) :
			ClauseParser(tokens, declarations) {};

	/* Parses this with parser's tokens into a with clause. */
	shared_ptr<WithClause> parse();
};