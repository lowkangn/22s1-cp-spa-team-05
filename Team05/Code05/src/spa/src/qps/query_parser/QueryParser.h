#pragma once

#include <qps/exceptions/PQLSyntaxError.h>
#include <qps/exceptions/PQLSemanticError.h>
#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query/Query.h>
#include <qps/query/clause/PatternClause.h>
#include <qps/query_parser/parsers/WithParser.h>
#include <list>
#include <unordered_map>

using namespace std;

/**
 * This class handles the creation of a Query object from a list of PQLTokens.
 */
class QueryParser {
private:
    list<PQLToken> tokens;
    list<shared_ptr<RelationshipClause>> suchThatClauses;
    list<shared_ptr<PatternClause>> patternClauses;
    list<shared_ptr<WithClause>> withClauses;

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

    void parseConstraints(unordered_map<string, ArgumentType>& declarations);

    shared_ptr<RelationshipClause> parseSuchThat(unordered_map<string, ArgumentType>& declarations);

	shared_ptr<PatternClause> parsePattern(unordered_map<string, ArgumentType>& declarations);

    shared_ptr<WithClause> parseWith(unordered_map<string, ArgumentType>& declarations);

    /* A friend class to access private fields for testing */
    friend class QueryParserAccessor;
};
