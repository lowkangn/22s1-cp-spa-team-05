#include <qps/query_parser/parsers/SuchThatClauseParser.h>

shared_ptr<RelationshipClause> SuchThatClauseParser::parse() {
    PQLToken clauseTypeToken = this->tokens.front();
    bool isCorrectType = isCorrectClauseType(clauseTypeToken);
    assert(isCorrectType);
    this->tokens.pop_front();

    list<ClauseArgument> args = extractArguments();
    this->checkArguments(args);
    this->isParseCompleted = true;
    return createClause(clauseTypeToken, args);
}

list<ClauseArgument> SuchThatClauseParser::extractArguments() {
    // check '('
    consumeOpenBracket();

    // get first arg and check ','
    ClauseArgument firstArg = parseOneStmtRefOrEntRef();
    consumeComma();

    // get second arg and check ')'
    ClauseArgument secondArg = parseOneStmtRefOrEntRef();
    consumeCloseBracket();

    return list<ClauseArgument>{firstArg, secondArg};
}
