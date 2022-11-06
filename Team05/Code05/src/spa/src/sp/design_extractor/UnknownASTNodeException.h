#pragma once
#include <stdexcept>

using namespace std;

/*
	A generic exception class for parser exceptions. All parser-related exceptions should inherit from this.
*/
class UnknownASTNodeTypeException : public logic_error {
    using logic_error::logic_error;
};
