#include <qps/query_parser/parsers/ModifiesParser.h>

bool ModifiesParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isModifies();
}

void ModifiesParser::checkArguments(list<ClauseArgument>& args) {
	//Modifies clause should have exactly 2 arguments
	assert(args.size() == 2);

	// first arg cannot be wildcard or variable
	ClauseArgument arg = args.front();
	if (arg.isWildcard() || arg.isVariableSynonym()) {
		throw PQLError("First arg for Modifies cannot be wildcard or variable");
	}

	// second arg must be wildcard or entRef
	arg = args.back();
	if (!arg.isWildcard() && !arg.isEntRefNoWildcard()) {
		throw PQLError("Second arg for Modifies must be an entRef");
	}
}

shared_ptr<SuchThatClause> ModifiesParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Modifies clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (args.front().isStmtRefNoWildcard()) {
		return shared_ptr<SuchThatClause>(new ModifiesSClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<SuchThatClause>(new ModifiesPClause(args.front(), args.back()));
	}
}
