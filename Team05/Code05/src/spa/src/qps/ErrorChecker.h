#pragma once

#include "PQLToken.h"
#include <vector>

using namespace std;

/**
 * This class handles the checking of a PQL Query that has been converted into a list of tokens for errors.
 */
class ErrorChecker {

public:

    /**
     * Takes in a vector of PQLTokens and throws a PQLError if a syntactic or semantic error is found.
     *
     * @param tokens to be checked for syntactic or semantic errors.
     */
    void check(vector<PQLToken> tokens);

};


