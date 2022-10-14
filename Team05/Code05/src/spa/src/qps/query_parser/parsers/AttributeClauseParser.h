#pragma once

#include <qps/query_parser/parsers/ClauseParser.h>

class AttributeClauseParser : public ClauseParser {
protected:

	/* Parses the attrName portion of an attrRef of the form synonym.attrName */
	ClauseArgument parseAttribute(ClauseArgument synonym);

	/* Checks for a dot and consumes it */
	void consumeDot();

	/* Checks the semantic compatibility of the synonym and the attribute */
	void checkSynonymAttributeCompatible(ClauseArgument& synonym, ClauseArgument& attribute);

	/* Returns true if the synonym-attribute pair is not the default attribute of the synonym */
	bool isNonDefaultAttribute(vector<ClauseArgument>& args);

	AttributeClauseParser(list<PQLToken> tokens, unordered_map<string, ArgumentType> declarations)
		: ClauseParser(tokens, declarations) {}

};