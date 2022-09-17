#include <qps/query_parser/parsers/PatternParser.h>

shared_ptr<PatternClause> PatternParser::parse() {
	PQLToken clauseTypeToken = this->tokens.front();
	assert(isCorrectClauseType(clauseTypeToken));

	vector<ClauseArgument> args = extractArguments();
	this->checkArguments(args);
	this->isParseCompleted = true;
	return createClause(clauseTypeToken, args);
}

vector<ClauseArgument> PatternParser::extractArguments() {
	// output arguments
	vector<ClauseArgument> output;

	// get first arg
	ClauseArgument firstArg = parseOneArgument();
	output.push_back(firstArg);

	// check '('
	consumeOpenBracket();

	while (!this->tokens.empty()) {
		// get next arg
		ClauseArgument nextArg = parseOneArgument();
		output.push_back(nextArg);

		// if close bracket next, all args parsed, so can exit
		if (this->tokens.front().isCloseBracket()) {
			break;
		}

		// check ','
		consumeComma();
	}

	// check ')'
	consumeCloseBracket();

	return output;
}
