#include <qps/query_parser/parsers/NextParser.h>

bool NextParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	if (tokens.size() == 1) {
		throw PQLSyntaxError("Query ended after 'Next'");
	}

	PQLToken secondToken = *std::next(this->tokens.begin());
	if (secondToken.isStarModifier()) {
		this->isStar = true;
		this->tokens.pop_front();
	}

	return clauseTypeToken.isNext();
}

void NextParser::checkArguments(list<ClauseArgument>& args) {
	// Next clause should have exactly 2 arguments
	assert(args.size() == 2);

	for (ClauseArgument arg : args) {
		if (arg.isStringLiteral()) {
			throw PQLSyntaxError("Both args for Next/Next* must be stmtRefs, which cannot be a string");
		}

		if (!arg.isWildcard() && !arg.isStmtRefNoWildcard()) {
			this->semanticErrorMessage = "Both args for Next/Next* must be stmtRefs";
		}
	}
}

shared_ptr<RelationshipClause> NextParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Next clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (this->isStar) {
		return shared_ptr<RelationshipClause>(new NextTClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<RelationshipClause>(new NextClause(args.front(), args.back()));
	}
}
