#pragma once

#include <string>

/**
 * This class encapsulates error messages for errors during parsing of query strings.
 */
class PQLError {

private:
    /**
     * String representation of the error.
     */
    std::string message;

public:

    /**
     * Returns error message string.
     *
     * @return error message string.
     */
    virtual std::string getMessage();
};
