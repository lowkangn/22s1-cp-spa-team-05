#include <qps/query_parser/parsers/ModifiesParser.h>

bool ModifiesParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isModifies();
}

void ModifiesParser::checkArguments(list<ClauseArgument>& args) {
	//Modifies clause should have exactly 2 arguments
	assert(args.size() == 2);

	// first arg cannot be wildcard, variable or constant
	ClauseArgument arg = args.front();
	if (arg.isWildcard() || arg.isVariableSynonym() || arg.isConstantSynonym()) {
		this->semanticErrorMessage = this->isSemanticallyValid()
			? "First arg for Uses cannot be wildcard, variable or constant"
			: this->semanticErrorMessage;
	}

	// second arg must be wildcard or a variable
	arg = args.back();
	if (!arg.isWildcard() && !arg.isVariableSynonym() && !arg.isStringLiteral()) {
		this->semanticErrorMessage = "Second arg for Modifies must be a wildcard or variable";
	}
}

shared_ptr<RelationshipClause> ModifiesParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Modifies clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (args.front().isStmtRefNoWildcard()) {
		return shared_ptr<RelationshipClause>(new ModifiesSClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<RelationshipClause>(new ModifiesPClause(args.front(), args.back()));
	}
}
