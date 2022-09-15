#include <qps/query_parser/parsers/SelectParser.h>

shared_ptr<SelectClause> SelectParser::parse() {
	PQLToken clauseTypeToken = this->tokens.front();
	assert(isCorrectClauseType(clauseTypeToken));
	this->tokens.pop_front();

	list<ClauseArgument> args = extractArguments();
	this->checkArguments(args);
	this->isParseCompleted = true;
	return createClause(clauseTypeToken, args);
}

bool SelectParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isSelect();
}

list<ClauseArgument> SelectParser::extractArguments() {
	if (this->tokens.empty()) {
		throw PQLError("Expected synonym after Select, reached end of query instead");
	}
	PQLToken token = this->tokens.front();

	if (!token.isName()) {
		throw PQLError("Expected synonym after select, got: " + token.getTokenString());
	}

	return list<ClauseArgument>{parseSynonym()};
}

void SelectParser::checkArguments(list<ClauseArgument>& args) {
	// Select clause has only 1 argument (for BasicSPA)
	assert(args.size() == 1);

	if (!args.front().isSynonym()) {
		throw PQLError("Argument for Select should be a synonym");
	}
}

shared_ptr<SelectClause> SelectParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	return shared_ptr<SelectClause>(new SelectClause(args.front()));
}
