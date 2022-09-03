#include "QueryParser.h"
#include <Declaration.h>
#include <DeclarationParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(tokens);
    list<Declaration> declarations = declParser.parse();

    list<Clause> selectClause;
    list<Clause> constraintClauses;
    /*
    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    list<Clause> selectClause = selParser.parse();

    ConstraintParser constraintParser = ConstraintParser(SelectParser.getRemainingTokens(), declarations);
    list<Clause> constraintClauses = constraintParser.parse();
    */

    return Query(selectClause, constraintClauses);
}