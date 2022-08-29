#pragma once

#include "PQLToken.h"
#include "Query.h"
#include <vector>

/**
 * This class handles the creation of a Query object from a list of PQLTokens.
 */
class QueryParser {

public:

    /**
     * Parses list of PQLTokens into a Query object.
     *
     * @param tokens used to create Query object.
     * @return Query object.
     */
    static Query parse(std::vector<PQLToken> tokens);
};

