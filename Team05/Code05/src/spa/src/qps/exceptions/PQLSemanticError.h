#pragma once

#include <string>

using namespace std;

/**
 * This class encapsulates error messages for semantic errors during parsing of query strings.
 * Refer to https://github.com/nus-cs3203/project-wiki/wiki/Basic-SPA-Program-Query-Language#other-rules
 * for a list of semantic rules.
 */
class PQLSemanticError : public exception {

private:
    /**
     * String representation of the error.
     */
    string message;

public:

    /**
     * Constructor for a PQLSemanticError.
     *
     * @param message
     */
    explicit PQLSemanticError(string message) {
        this->message = message;
    }

    /**
     * Returns error message string.
     *
     * @return error message string.
     */
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }

};
