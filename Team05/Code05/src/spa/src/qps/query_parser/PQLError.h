#pragma once

#include <string>

using namespace std;

/**
 * This class encapsulates error messages for errors during parsing of query strings.
 */
class PQLError : public exception {

private:
    /**
     * String representation of the error.
     */
    string message;

public:

    /**
     * Returns error message string.
     *
     * @return error message string.
     */
    virtual string getMessage();
};
