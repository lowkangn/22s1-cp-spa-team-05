#include <qps/query_parser/parsers/SemanticChecker.h>

bool SemanticChecker::isSemanticallyValid() {
	string message = this->semanticErrorMessage;
	return message == NO_SEMANTIC_ERROR_MESSAGE;
}

string SemanticChecker::getSemanticErrorMessage() {
	assert(!isSemanticallyValid());
	return this->semanticErrorMessage;
}
