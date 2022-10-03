#include <qps/query_parser/parsers/PatternParser.h>

shared_ptr<PatternClause> PatternParser::parse() {
	PQLToken clauseTypeToken = this->tokens.front();

	vector<ClauseArgument> args = extractArguments();
	this->checkArguments(args);
	this->isParseCompleted = true;
	return createClause(clauseTypeToken, args);
}
