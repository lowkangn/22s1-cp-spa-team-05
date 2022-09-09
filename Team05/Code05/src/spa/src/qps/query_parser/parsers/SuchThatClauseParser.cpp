#include <qps/query_parser/parsers/SuchThatClauseParser.h>

list<ClauseArgument> SuchThatClauseParser::extractArguments() {
	// check '('
	consumeOpenBracket();

	// get first arg and check ','
	ClauseArgument firstArg = parseOneArgument();
	consumeComma();

	// get second arg and check ')'
	ClauseArgument secondArg = parseOneArgument();
	consumeCloseBracket();
	
	return list<ClauseArgument>{firstArg, secondArg};
}

