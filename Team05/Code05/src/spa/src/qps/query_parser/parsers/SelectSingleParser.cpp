#include <qps/query_parser/parsers/SelectSingleParser.h>

list<ClauseArgument> SelectSingleParser::extractArguments() {
    ClauseArgument synonym = parseSynonym();
    list<ClauseArgument> args = {synonym};
    if (!this->tokens.empty() && this->tokens.front().isDot()) {
        this->consumeDot();
        ClauseArgument attribute = this->parseAttribute(synonym);
        args.push_back(attribute);
        this->checkSynonymAttributeCompatible(synonym, attribute);
    }
    return args;
}

shared_ptr<SelectClause> SelectSingleParser::createClause(list<ClauseArgument>& args) {
    return make_shared<SelectClause>(SelectClause::createSynonymSelectClause(args));
}

bool SelectSingleParser::isCorrectClauseType(PQLToken clauseTypeToken) {
    return clauseTypeToken.isName();
}
