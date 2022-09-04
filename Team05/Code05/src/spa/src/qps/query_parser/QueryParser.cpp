#include "QueryParser.h"
#include <DesignEntity.h>
#include <DeclarationParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(tokens);
    unordered_map<string, DesignEntity> declarations = declParser.parse();

    Clause selectClause;
    list<Clause> constraintClauses;
    /* TODO:
    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    Clause selectClause = selParser.parse();

    ConstraintParser constraintParser = ConstraintParser(SelectParser.getRemainingTokens(), declarations);
    list<Clause> constraintClauses = constraintParser.parse();
    */

    return Query(selectClause, constraintClauses);
}