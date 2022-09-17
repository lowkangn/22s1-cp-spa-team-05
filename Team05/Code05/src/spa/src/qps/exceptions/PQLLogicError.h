#pragma once

#include <string>

using namespace std;

/**
 * This class encapsulates error messages for logic errors at any point in the QPS
 */
class PQLLogicError : public exception {

private:
    /**
     * String representation of the error.
     */
    string message;

public:

    /**
     * Constructor for a PQLLogicError.
     *
     * @param message
     */
    explicit PQLLogicError(string message) {
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
