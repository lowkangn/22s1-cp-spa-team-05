#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(this->tokens);
    unordered_map<string, ArgumentType> declarations = declParser.parse();

    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    shared_ptr<SelectClause> selectClause = selParser.parse();

    this->tokens = selParser.getRemainingTokens();
    if (this->tokens.empty()) {
        return Query(selectClause, list<shared_ptr<SuchThatClause>>{});
    }
    list<shared_ptr<SuchThatClause>> constraintClauses = parseConstraints(declarations);

    return Query(selectClause, constraintClauses);
}

list<shared_ptr<SuchThatClause>> QueryParser::parseConstraints(unordered_map<string, ArgumentType> declarations) {
    list<shared_ptr<SuchThatClause>> clauses;
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

shared_ptr<SuchThatClause> QueryParser::parseSuchThat(unordered_map<string, ArgumentType> declarations) {
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
    shared_ptr<SuchThatClause> clause = parserPointer->parse();
    this->tokens = parserPointer->getRemainingTokens();
    return clause;
}