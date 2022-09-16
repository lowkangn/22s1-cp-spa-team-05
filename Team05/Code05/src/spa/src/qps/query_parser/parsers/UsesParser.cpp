#include <qps/query_parser/parsers/UsesParser.h>

bool UsesParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isUses();
}

void UsesParser::checkArguments(list<ClauseArgument>& args) {
	//Uses clause should have exactly 2 arguments
	assert(args.size() == 2);

	// first arg cannot be wildcard or variable
	ClauseArgument arg = args.front();
	if (arg.isWildcard() || arg.isVariableSynonym()) {
		throw PQLError("First arg for Uses cannot be wildcard or variable");
	}

	// second arg must be wildcard or entRef
	arg = args.back();
	if (!arg.isWildcard() && !arg.isEntRefNoWildcard()) {
		throw PQLError("Second arg for Uses must be an entRef");
	}
}

shared_ptr<RelationshipClause> UsesParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Uses clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (args.front().isStmtRefNoWildcard()) {
		return shared_ptr<RelationshipClause>(new UsesSClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<RelationshipClause>(new UsesPClause(args.front(), args.back()));
	}
}