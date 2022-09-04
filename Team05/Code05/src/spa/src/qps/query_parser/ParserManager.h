#pragma once

#include "QPSErrorChecker.h"
#include "../query/Query.h"
#include "QueryParser.h"
#include "QPSTokenizer.h"
#include <fstream>

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

public:

    /**
     * Takes in a PQL query in the form of a stream and returns a Query object.
     *
     * @param queryText The input stream used to create the Query.
     * @return Query object generated, provided the query stream has no errors.
     */
    Query parse(istream& queryStream);

};
