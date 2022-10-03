#include <qps/query_parser/parsers/SelectSingleParser.h>

list<ClauseArgument> SelectSingleParser::extractArguments() {
	return list<ClauseArgument>{parseSynonym()};
}

shared_ptr<SelectClause> SelectSingleParser::createClause(list<ClauseArgument>& args) {
	return make_shared<SelectClause>(SelectClause::createSynonymSelectClause(args));
}

bool SelectSingleParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isName();
}
