#include <qps/query_parser/parsers/SelectSingleParser.h>

list<ClauseArgument> SelectSingleParser::extractArguments() {
	ClauseArgument arg = parseOneArgument();
	if (!arg.isSynonym()) {
		throw PQLSyntaxError("Select tuple should only have synonyms");
	}
	return list<ClauseArgument>{arg};
}

shared_ptr<SelectClause> SelectSingleParser::createClause(list<ClauseArgument>& args) {
	return make_shared<SelectClause>(SelectClause::createSynonymSelectClause(args));
}

bool SelectSingleParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isName();
}
