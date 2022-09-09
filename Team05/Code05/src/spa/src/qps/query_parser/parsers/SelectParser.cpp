#include <qps/query_parser/parsers/SelectParser.h>

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

shared_ptr<Clause> SelectParser::createClause(PQLToken clauseTypeToken, list<ClauseArgument>& args) {
	return shared_ptr<Clause>(new SelectClause(args.front()));
}