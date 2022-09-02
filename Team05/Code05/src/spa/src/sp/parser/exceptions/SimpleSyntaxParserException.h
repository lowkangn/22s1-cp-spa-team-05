#pragma once
#include <stdexcept>
using namespace std;

/*
	A generic exception class for parser exceptions. All parser-related exceptions should inherit from this.
*/
class SimpleSyntaxParserException : public logic_error {
	using  logic_error::logic_error;
};

/*
	Thrown when a rule is not initialized with tokens.
*/
class SimpleSyntaxRuleNotInitializedException : public exception {

};