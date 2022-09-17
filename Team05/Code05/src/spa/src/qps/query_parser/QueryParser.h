#pragma once

#include <qps/exceptions/PQLSyntaxError.h>
#include <qps/exceptions/PQLSemanticError.h>
#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query/Query.h>
#include <list>
#include <unordered_map>

using namespace std;

/**
 * This class handles the creation of a Query object from a list of PQLTokens.
 */
class QueryParser {
private:
    list<PQLToken> tokens;
public:

    /**
     * Instantiates a QueryParser that will parse the given tokens.
     *
     * @param tokens to be parsed into a Query object.
     */
    QueryParser(list<PQLToken> tokens) {
        this->tokens = tokens;
    };

    /**
     * Parses list of PQLTokens into a Query object.
     * 
     * @return Query object.
     */
    Query parse();

    list<shared_ptr<RelationshipClause>> parseConstraints(unordered_map<string, ArgumentType> declarations);

    shared_ptr<RelationshipClause> parseSuchThat(unordered_map<string, ArgumentType> declarations);
};
