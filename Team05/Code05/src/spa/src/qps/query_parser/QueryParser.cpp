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
    shared_ptr<Clause> selectClause = selParser.parse();

    tokens = selParser.getRemainingTokens();
    list<shared_ptr<Clause>> constraintClauses = parseConstraints(declarations);

    return Query(selectClause, constraintClauses);
}

list<shared_ptr<Clause>> QueryParser::parseConstraints(unordered_map<string, DesignEntity> declarations) {
    list<shared_ptr<Clause>> clauses;
    PQLToken token = tokens.front();
    while (!tokens.empty()) {
        token = tokens.front();
        tokens.pop_front();
        if (token.isSuch()) {
            clauses.emplace_back(parseSuchThat(declarations));
        }
        else if (token.isPattern()) {
            //not needed for MVP
        }
    }
    return clauses;
}

shared_ptr<Clause> QueryParser::parseSuchThat(unordered_map<string, DesignEntity> declarations) {
    if (tokens.empty() || !tokens.front().isThat()) {
        throw PQLError("Missing 'that' after 'such'");
    }
    tokens.pop_front();
    if (tokens.empty() ) {
        throw PQLError("Missing relRef after such that");
    }
    PQLToken token = tokens.front();
    shared_ptr<SuchThatClauseParser> parserPointer;

    if (token.isModifies()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ModifiesParser(tokens, declarations));
    } else {
        //create other SuchThatClauseParsers 
        //not needed for MVP
    }
    shared_ptr<Clause> clause = parserPointer->parse();
    tokens = parserPointer->getRemainingTokens();
    return clause;
}