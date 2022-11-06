#pragma once
#include <stdexcept>

using namespace std;

/*
	A generic exception class for pkb exceptions. All parser-related exceptions should inherit from this.
*/
class PkbException : public logic_error {
    using logic_error::logic_error;
};

/*
	A generic exception class for pkb exceptions. All parser-related exceptions should inherit from this.
*/
class PkbNotFoundException : public logic_error {
    using logic_error::logic_error;
};
