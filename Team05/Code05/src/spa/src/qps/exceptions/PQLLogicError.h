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
     */
    explicit PQLLogicError(string message) {
        this->message = message;
    }

    /**
     * Returns error message string. Note: this is the method signature to 
     * correctly inherit the default exception class:
     * https://stackoverflow.com/questions/66313527/c-overriding-exceptionwhat
     *
     * @return error message string.
     */
    const char* what() const noexcept override {
        return message.c_str();
    }
};
