#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/DesignEntity.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(tokens);
    unordered_map<string, DesignEntity> declarations = declParser.parse();

    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    Clause selectClause = selParser.parse();

    tokens = SelectParser.getRemainingTokens();
    list<Clause> constraintClauses;
    /* TODO:
    list<Clause> constraintClauses = parseConstraints();
    */

    return Query(selectClause, constraintClauses);
}

list<Clause> QueryParser::parseConstraints() {
    list<Clause> clauses;
    PQLToken token = tokens.front();
    while (!tokens.empty()) {
        token = tokens.front();
        tokens.pop_front();
        if (token.isSuch()) {
            clauses.emplace_back(parseSuchThat());
        }
        else if (token.isPattern()) {
            //not needed for MVP
        }
    }
}

Clause QueryParser::parseSuchThat() {
    if (tokens.empty() || !tokens.front().isThat()) {
        throw PQLError("Missing 'that' after 'such'");
    }
    tokens.pop_front();
    if (tokens.empty() ) {
        throw PQLError("Missing relRef after such that");
    }
    PQLToken token = tokens.front();
    shared_ptr<ClauseParser> parserPointer;
    if (token.isModifies()) {
        parserPointer = shared_ptr<ClauseParser>(new ModifiesParser(tokens));
    }
    else {
        //create other suchthatParsers for 
        //not needed for MVP
    }
    shared_ptr<Clause> clause = shared_ptr<Clause>(parserPointer->parse());
    tokens = parserPointer->getRemainingTokens();
    return clause.get();
}