#include <qps/query_parser/parsers/SelectParser.h>

shared_ptr<SelectClause> SelectParser::parse() {
	PQLToken clauseTypeToken = this->tokens.front();
	assert(isCorrectClauseType(clauseTypeToken));
	this->tokens.pop_front();

	list<ClauseArgument> args = extractArguments();
	this->isParseCompleted = true;
	return createClause(clauseTypeToken, args);
}

bool SelectParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isSelect();
}

list<ClauseArgument> SelectParser::extractArguments() {
	if (this->tokens.empty()) {
		throw PQLSyntaxError("Expected synonym after Select, reached end of query instead");
	}
	PQLToken token = this->tokens.front();

	if (!token.isName()) {
		throw PQLSyntaxError("Expected synonym after select, got: " + token.getTokenString());
	}

	return list<ClauseArgument>{parseSynonym()};
}

shared_ptr<SelectClause> SelectParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	return shared_ptr<SelectClause>(new SelectClause(args.front()));
}
