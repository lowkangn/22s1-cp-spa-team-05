#pragma once

#include "PQLToken.h"
#include "../query/Query.h"
#include <list>

using namespace std;

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
    Query parse(list<PQLToken> tokens);
};

