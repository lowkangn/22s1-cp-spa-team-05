#pragma once

#include "PQLToken.h"
#include "../query/Query.h"
#include <list>
#include <unordered_set>

using namespace std;

/**
 * This class handles the creation of a Query object from a list of PQLTokens.
 */
class QueryParser {
private:
    list<PQLToken> tokens;
    unordered_set<string> names;
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

    list<Clause> QueryParser::parseConstraints();

    list<Clause> QueryParser::parseSuchThat();
};

