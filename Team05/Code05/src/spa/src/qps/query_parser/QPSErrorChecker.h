#pragma once

#include "PQLToken.h"
#include <list>

using namespace std;

/**
 * This class handles the checking of a PQL Query that has been converted into a list of tokens for errors.
 */
class QPSErrorChecker {

public:

    /**
     * Takes in a vector of PQLTokens and throws a PQLError if a syntactic or semantic error is found.
     *
     * @param tokens to be checked for syntactic or semantic errors.
     */
    void check(list<PQLToken> tokens);

};


