#pragma once

#include <qps/query_parser/parsers/SelectParser.h>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class SelectMultipleParser : public SelectParser {
private:
    /* Extracts the arguments of the clause. Consumes all tokens in the clause after 'Select' */
    list<ClauseArgument> extractArguments() override;

    /* Creates the clause given the clause type and arguments */
    shared_ptr<SelectClause> createClause(list<ClauseArgument>& args) override;

    /* Returns true if the clauseTypeToken matches the clause that the Parser parses */
    bool isCorrectClauseType(PQLToken clauseTypeToken) override;

    /* Parses one entry in the tuple of the Select Multiple clause. */
    void parseOneEntry(list<ClauseArgument>& argumentListToAddTo);

    /* Consumes the angled open bracket token. */
    void consumeAngledOpenBracket() {
        if (this->tokens.empty() || !this->tokens.front().isAngledOpenBracket()) {
            throw PQLSyntaxError("Expected angled open bracket");
        }
        this->tokens.pop_front();
        if (this->tokens.empty()) {
            throw PQLSyntaxError("Expected argument after angled open bracket");
        }
    }

    /* Consumes the angled close bracket token. */
    void consumeAngledCloseBracket() {
        if (this->tokens.empty() || !this->tokens.front().isAngledCloseBracket()) {
            throw PQLSyntaxError("Expected angled close bracket");
        }
        this->tokens.pop_front();
    }

public:
    SelectMultipleParser(
        list<PQLToken>& tokens,
        const unordered_map<string, ArgumentType>& declarations) : SelectParser(
        tokens,
        declarations) {
    }
};
