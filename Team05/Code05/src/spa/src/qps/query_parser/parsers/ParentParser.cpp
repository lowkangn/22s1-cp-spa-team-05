#include <qps/query_parser/parsers/ParentParser.h>

bool ParentParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	if (tokens.size() == 1) {
		throw PQLError("Query ended after 'Parent'");
	}

	PQLToken secondToken = *std::next(this->tokens.begin());
	if (secondToken.isStarModifier()) {
		this->isStar = true;
		this->tokens.pop_front();
	}

	return clauseTypeToken.isParent();
}

void ParentParser::checkArguments(list<ClauseArgument>& args) {
	//Parent clause should have exactly 2 arguments
	assert(args.size() == 2);


	for (ClauseArgument arg : args) {
		if (!arg.isWildcard() && !arg.isStmtRefNoWildcard()) {
			throw PQLError("Both args for Parent must be stmtRefs");
		}
	}

}

shared_ptr<RelationshipClause> ParentParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	//Parent clause should have exactly 2 arguments
	assert(args.size() == 2);

	if (this->isStar) {
		return shared_ptr<RelationshipClause>(new ParentTClause(args.front(), args.back()));
	} 
	else {
		return shared_ptr<RelationshipClause>(new ParentClause(args.front(), args.back()));
	}
}
