#include <qps/query_parser/parsers/SuchThatClauseParser.h>

shared_ptr<RelationshipClause> SuchThatClauseParser::parse() {
	PQLToken clauseTypeToken = extractClauseType();

	list<ClauseArgument> args = extractArguments();
	this->checkArguments(args);
	this->isParseCompleted = true;
	return createClause(clauseTypeToken, args);
}

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
