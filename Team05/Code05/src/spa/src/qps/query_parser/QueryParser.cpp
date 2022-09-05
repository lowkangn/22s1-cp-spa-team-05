#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/DesignEntity.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(this->tokens);
    unordered_map<string, DesignEntity> declarations = declParser.parse();

    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    shared_ptr<Clause> selectClause = selParser.parse();

    this->tokens = selParser.getRemainingTokens();
    if (this->tokens.empty()) {
        return Query(selectClause, list<shared_ptr<Clause>>{});
    }
    list<shared_ptr<Clause>> constraintClauses = parseConstraints(declarations);

    return Query(selectClause, constraintClauses);
}

list<shared_ptr<Clause>> QueryParser::parseConstraints(unordered_map<string, DesignEntity> declarations) {
    list<shared_ptr<Clause>> clauses;
    PQLToken token = this->tokens.front();
    while (!this->tokens.empty()) {
        token = this->tokens.front();
        this->tokens.pop_front();
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
    if (this->tokens.empty() || !this->tokens.front().isThat()) {
        throw PQLError("Missing 'that' after 'such'");
    }
    this->tokens.pop_front();
    if (this->tokens.empty() ) {
        throw PQLError("Missing relRef after such that");
    }
    PQLToken token = this->tokens.front();
    shared_ptr<SuchThatClauseParser> parserPointer;

    if (token.isModifies()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ModifiesParser(this->tokens, declarations));
    } else {
        //create other SuchThatClauseParsers 
        //not needed for MVP
    }
    shared_ptr<Clause> clause = parserPointer->parse();
    this->tokens = parserPointer->getRemainingTokens();
    return clause;
}