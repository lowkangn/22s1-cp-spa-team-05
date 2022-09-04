#include <assert.h>
#include <qps/query_parser/parsers/ModifiesParser.h>

shared_ptr<Clause> ModifiesParser::parse() {
	PQLToken token = tokens.front();
	assert(token.isModifies());
	tokens.pop_front();
	pair <ClauseArgument,ClauseArgument> args = extractArguments();
	
	// first arg cannot be wildcard
	if (args.first.isWildcard()) {
		throw PQLError("First arg for Modifies cannot be wildcard");
	}

	// second arg must be wildcard or entRef
	if (!args.second.isWildcard() && !args.second.isEntRefNoWildcard()) {
		throw PQLError("Second arg for Modifies must be an entRef");
	}

	// decide which Modifies
	if (args.first.isStmtRefNoWildcard()) {
		return shared_ptr<Clause>(new ModifiesSClause(args.first, args.second));
	} else {
		return shared_ptr<Clause>(new ModifiesPClause(args.first, args.second));
	}
}
