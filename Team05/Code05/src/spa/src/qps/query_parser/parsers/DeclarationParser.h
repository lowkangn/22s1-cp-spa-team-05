#pragma once

#include <qps/exceptions/PQLSemanticError.h>
#include <qps/exceptions/PQLSyntaxError.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SemanticChecker.h>
#include <qps/query_tokenizer/PQLToken.h>

#include <list>
#include <string>
#include <unordered_map>

class DeclarationParser : public SemanticChecker {
private:
    list<PQLToken>& tokens;
    unordered_map<string, ArgumentType> declarations;

    /**
     * Parses one declaration clause, defined
     * as "design-entity synonym (',' synonym)* ';'"
     *
     * Preconditions: this->Tokens is non-empty
     */
    void parseOneDeclaration();

    bool isSynonymAlreadyDeclared(string synonym);

public:
    explicit DeclarationParser(list<PQLToken>& tokens) : tokens(tokens) {
        this->semanticErrorMessage = NO_SEMANTIC_ERROR_MESSAGE;
    }

    unordered_map<string, ArgumentType> getDeclarations() {
        return this->declarations;
    }

    /* Parses all declarations in the query. */
    unordered_map<string, ArgumentType> parse();
};
