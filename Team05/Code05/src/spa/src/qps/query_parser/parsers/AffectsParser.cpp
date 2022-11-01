#include <qps/query_parser/parsers/AffectsParser.h>

bool AffectsParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	if (tokens.size() == 1) {
		throw PQLSyntaxError("Query ended after 'Affects'");
	}

	PQLToken secondToken = *std::next(this->tokens.begin());
	if (secondToken.isStarModifier()) {
		this->isStar = true;
		this->tokens.pop_front();
	}

	return clauseTypeToken.isAffects();
}

void AffectsParser::checkArguments(list<ClauseArgument>& args) {
	// Affects clause should have exactly 2 arguments
	assert(args.size() == 2);

	for (ClauseArgument arg : args) {
		if (arg.isStringLiteral()) {
			throw PQLSyntaxError("Both args for Affects/Affects* must be statements, which cannot be a string");
		}

		if (!arg.isWildcard() && !arg.isStmtRefNoWildcard() && !arg.isLineNumber()) {
			this->semanticErrorMessage = "Both args for Affects/Affects* must be stmtRefs";
		}
	}
}

shared_ptr<RelationshipClause> AffectsParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Affects clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (this->isStar) {
		return shared_ptr<RelationshipClause>(new AffectsTClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<RelationshipClause>(new AffectsClause(args.front(), args.back()));
	}
}
