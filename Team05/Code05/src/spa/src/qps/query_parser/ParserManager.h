#pragma once

#include "QPSErrorChecker.h"
#include "Query.h"
#include "QueryParser.h"
#include "QPSTokenizer.h"
#include <string>

using namespace std;

/**
 * This class handles the conversion of a query string to a Query object.
 */
class ParserManager {

private:

    /**
     * Tokenizer for converting the query string into a list of tokens.
     */
    QPSTokenizer tokenizer;

    /**
     * ErrorChecker for validating the grammar of the query.
     */
    QPSErrorChecker checker;

    /**
     * QueryParser for converting the list of tokens into a Query object.
     */
    QueryParser parser;

public:

    /**
     * Takes in a PQL query in the form of a string and returns a Query object.
     *
     * @param queryText The input string used to create the Query.
     * @return Query object generated, provided the query string has no errors.
     */
    Query parse(string queryText);

};
