#include <qps/query_parser/parsers/PatternAssignParser.h>

bool PatternAssignParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return declarations.at(clauseTypeToken.getTokenString()) == ArgumentType::ASSIGN;
}

void PatternAssignParser::checkArguments(vector<ClauseArgument>& args) {
	//Pattern clause should have exactly 3 arguments
	assert(args.size() == 3);

	// first arg must be assignment
	ClauseArgument arg = args[0];
	if (!arg.isAssignSynonym()) {
		this->semanticErrorMessage = "First arg for assign pattern must be assign";
	}

	// second arg must be wildcard or string literal or variable
	arg = args[1];
	if (!arg.isWildcard() && !arg.isVariableSynonym() && !arg.isStringLiteral()) {
		this->semanticErrorMessage = "Second arg for pattern must be a variable synonym, string name, or wildcard";
	}

	// third arg must be wildcard or string literal or pattern string (with wildcards)
	arg = args[2];
	if (!arg.isWildcard() && !arg.isStringLiteral() && !arg.isPatternString() && !arg.isPatternStringWithWildcards()) {
		throw this->semanticErrorMessage = "Third arg for pattern must be a string literal or wildcard or string with wildcards";
	}
}

shared_ptr<PatternClause> PatternAssignParser::createClause(PQLToken clauseTypeToken, vector<ClauseArgument>& args) {
	//Pattern clause should have exactly 3 arguments
	assert(args.size() == 3);

	return shared_ptr<PatternClause>(new PatternAssignClause(args[0], args[1], args[2]));
}

vector<ClauseArgument> PatternAssignParser::extractArguments() {
	vector<ClauseArgument> output;

	// get first arg
	ClauseArgument firstArg = parseOneStmtRefOrEntRef();
	output.push_back(firstArg);

	// check '('
	consumeOpenBracket();

	// get second arg
	ClauseArgument secondArg = parseOneStmtRefOrEntRef();
	output.push_back(secondArg);

	// check ','
	consumeComma();

	if (this->tokens.front().isUnderscore()) {
		PQLToken wildcardToken = this->tokens.front();
		this->tokens.pop_front();

		if (this->tokens.empty() || !this->tokens.front().isQuote()) {
			ClauseArgument thirdArg = ClauseArgument::createWildcardArg();
			output.push_back(thirdArg);
		} else {
			ClauseArgument thirdArg = parsePatternStringWithWildcards();
			output.push_back(thirdArg);
		}
	} else if (this->tokens.front().isQuote()) {
		ClauseArgument thirdArg = parsePatternString();
		output.push_back(thirdArg);
	} else {
		throw PQLSyntaxError("Third argument to PatternAssign should start with quote or underscore");
	}

	// check ')'
	consumeCloseBracket();

	return output;
}