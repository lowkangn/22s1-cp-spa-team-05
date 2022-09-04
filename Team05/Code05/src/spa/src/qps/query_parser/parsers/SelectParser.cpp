#include <qps/query_parser/parsers/SelectParser.h>

SelectClause* SelectParser::parse() {
	PQLToken token = tokens.front();
	
	//check for Select token
	if (!token.isSelect()) {
		throw PQLError("Expected Select, got:");
	}
	tokens.pop_front();
	
	//check for synonym name after Select
	if (tokens.empty()) {
		throw PQLError("Expected synonym after Select, reached end of query instead");
	}
	token = tokens.front();
	if (!token.isName()) {
		throw PQLError("Expected synonym after select, got: " + token.getTokenString());
	}
	tokens.pop_front();

	return new SelectClause(parseSynonym(token));
}