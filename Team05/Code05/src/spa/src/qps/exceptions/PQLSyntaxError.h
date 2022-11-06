#pragma once

#include <string>

using namespace std;

/**
 * This class encapsulates error messages for syntax errors during parsing of query strings.
 * Refer to
 * https://github.com/nus-cs3203/project-wiki/wiki/Basic-SPA-Program-Query-Language#basic-pql-grammar
 * for a list of syntax rules.
 */
class PQLSyntaxError : public exception {
private:
    /**
     * String representation of the error.
     */
    string message;

public:
    /**
     * Constructor for a PQLSyntaxError.
     */
    explicit PQLSyntaxError(string message) {
        this->message = message;
    }

    /**
     * Returns error message string.Note: this is the method signature to 
     * correctly inherit the default exception class:
     * https://stackoverflow.com/questions/66313527/c-overriding-exceptionwhat
     *
     * @return error message string.
     */
    const char* what() const noexcept override {
        return message.c_str();
    }
};
