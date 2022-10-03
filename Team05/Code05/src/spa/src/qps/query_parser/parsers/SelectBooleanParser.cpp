#include <qps/query_parser/parsers/SelectBooleanParser.h>

list<ClauseArgument> SelectBooleanParser::extractArguments() {
	PQLToken booleanKeywordToken = this->tokens.front();
	this->tokens.pop_front();
	return list<ClauseArgument>{};
}

shared_ptr<SelectClause> SelectBooleanParser::createClause(list<ClauseArgument>& args) {
	return make_shared<SelectClause>(SelectClause::createBooleanSelectClause());
}

bool SelectBooleanParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isBooleanKeyword();
}