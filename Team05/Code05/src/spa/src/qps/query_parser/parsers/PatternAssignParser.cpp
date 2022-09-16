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
		throw PQLError("First arg for pattern cannot be wildcard or variable");
	}

	// second arg must be wildcard or entRef
	arg = args[1];
	if (!arg.isWildcard() && !arg.isEntRefNoWildcard()) {
		throw PQLError("Second arg for pattern must be an entRef or wildcard");
	}

	// third arg must be wildcard or string literal
	arg = args[2];
	if (!arg.isWildcard() && !arg.isStringLiteral()) {
		throw PQLError("Third arg for pattern must be a string literal or wildcard");
	}
}

shared_ptr<PatternClause> PatternAssignParser::createClause(PQLToken clauseTypeToken, vector<ClauseArgument>& args) {
	//Pattern clause should have exactly 3 arguments
	assert(args.size() == 3);

	return shared_ptr<PatternClause>(new PatternAssignClause(args));
}
