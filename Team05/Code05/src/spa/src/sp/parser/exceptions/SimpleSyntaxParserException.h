using namespace std;

/*
	A generic exception class for parser exceptions. All parser-related exceptions should inherit from this.
*/
class SimpleSyntaxParserException : public exception{

};

/*
	Thrown when a rule is not initialized with tokens.
*/
class SimpleSyntaxRuleNotInitializedException : public SimpleSyntaxParserException {

};