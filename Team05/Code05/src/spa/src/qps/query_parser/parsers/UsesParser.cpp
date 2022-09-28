#include <qps/query_parser/parsers/UsesParser.h>

bool UsesParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isUses();
}

void UsesParser::checkArguments(list<ClauseArgument>& args) {
	//Uses clause should have exactly 2 arguments
	assert(args.size() == 2);

	// first arg cannot be wildcard, variable or constant
	ClauseArgument arg = args.front();
	if (arg.isWildcard() || arg.isVariableSynonym() || arg.isConstantSynonym()) {
		this->semanticErrorMessage = this-> isSemanticallyValid() 
			? "First arg for Uses cannot be wildcard, variable or constant" 
			: this->semanticErrorMessage;
	}

	arg = args.back();

	// syntactically, second arg must be an entRef which cannot be a number
	if (arg.isLineNumber()) {
		throw PQLSyntaxError("Second arg for Uses cannot be a number");
	}

	// semantically, second arg must be a wildcard or a variable
	if (!arg.isWildcard() && !arg.isVariableSynonym() && !arg.isStringLiteral()) {
		this->semanticErrorMessage = "Second arg for Uses must be a wildcard or variable";
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
