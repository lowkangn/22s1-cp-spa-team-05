#include <qps/query_parser/parsers/FollowsParser.h>

bool FollowsParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	if (tokens.size() == 1) {
		throw PQLError("Query ended after 'Follows'");
	}

	PQLToken secondToken = *std::next(this->tokens.begin());
	if (secondToken.isStarModifier()) {
		this->isStar = true;
		this->tokens.pop_front();
	}

	return clauseTypeToken.isFollows();
}

void FollowsParser::checkArguments(list<ClauseArgument>& args) {
	//Follows clause should have exactly 2 arguments
	assert(args.size() == 2);


	for (ClauseArgument arg : args) {
		if (!arg.isWildcard() && !arg.isStmtRefNoWildcard()) {
			throw PQLError("Both args for Follows must be stmtRefs");
		}
	}

}

shared_ptr<RelationshipClause> FollowsParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Follows clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (this->isStar) {
		return shared_ptr<RelationshipClause>(new FollowsTClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<RelationshipClause>(new FollowsClause(args.front(), args.back()));
	}
}