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
     * Constructor for a PQLError.
     *
     * @param message
     */
    explicit PQLError(string message) {
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
