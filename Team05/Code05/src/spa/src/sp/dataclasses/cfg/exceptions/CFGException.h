#pragma once
#include <stdexcept>
using namespace std;

/*
	A generic exception class for CFG exceptions.
*/
class CFGException : public logic_error {
    using logic_error::logic_error;
};
