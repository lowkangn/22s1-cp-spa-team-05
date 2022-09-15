#include <qps/query_parser/parsers/ParentParser.h>

PQLToken ParentParser::extractClauseType() {
	PQLToken clauseTypeToken = this->tokens.front();
	assert(clauseTypeToken.isParent());
	this->tokens.pop_front();
	if (this->tokens.front().isStarModifier()) {
		clauseTypeToken = this->tokens.front();
		this->tokens.pop_front();
	}
	return clauseTypeToken;
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
	assert(clauseTypeToken.isParent() || clauseTypeToken.isStarModifier());

	if (clauseTypeToken.isParent()) {
		return shared_ptr<RelationshipClause>(new ParentClause(args.front(), args.back()));
	}
	else {
		return shared_ptr<RelationshipClause>(new ParentTClause(args.front(), args.back()));
	}
}
