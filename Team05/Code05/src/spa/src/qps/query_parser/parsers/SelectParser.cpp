#include <qps/query_parser/parsers/SelectParser.h>

shared_ptr<SelectClause> SelectParser::parse() {
    PQLToken clauseTypeToken = this->tokens.front();
    assert(isCorrectClauseType(clauseTypeToken));

    list<ClauseArgument> args = extractArguments();
    this->isParseCompleted = true;
    return createClause(args);
}
