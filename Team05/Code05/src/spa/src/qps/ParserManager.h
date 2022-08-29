#pragma once

#include "Query.h"
#include <string>

/**
 * This class handles the conversion of a query string to a Query object.
 */
class ParserManager {

public:

    /**
     * Takes in a PQL query in the form of a string and returns a Query object.
     *
     * @param queryText The input string used to create the Query.
     * @return Query object generated, provided the query string has no errors.
     */
    static Query parse(std::string queryText);

};
