#pragma once

#include <vector>
#include "PQLToken.h"

using namespace std;

/**
 * This class handles the conversion of a given query string into PQLTokens for error checking and conversion to a
 * Query object.
 */
class Tokenizer {

public:

    /**
     * Tokenizes the given query string into a list of PQLTokens.
     *
     * @param queryText to be converted into a list of PQLTokens.
     * @return list of PQLTokens representing the given query string.
     */
    vector<PQLToken> tokenize(string queryText);
};
