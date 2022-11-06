#pragma once

#include <assert.h>

#include <qps/exceptions/PQLSemanticError.h>

#include <string>

/* SemanticChecker is an abstract class for 
* the checking of semantics in a query. 
*/
class SemanticChecker {
protected:
    /* A message that indicates a semantic error, if any */
    string semanticErrorMessage;

    /* Protected constructor so that the class is abstract
     * even though it has no pure virtual methods. */
    SemanticChecker() = default;

public:
    /* Returns true if there is no semantic error, false otherwise */
    bool isSemanticallyValid();

    /* Returns the semanticErrorMessage if there is a semantic error */
    string getSemanticErrorMessage();
};
