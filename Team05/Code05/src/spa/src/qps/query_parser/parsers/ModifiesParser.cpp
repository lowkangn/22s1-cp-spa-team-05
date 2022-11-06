#include <qps/query_parser/parsers/ModifiesParser.h>

bool ModifiesParser::isCorrectClauseType(PQLToken clauseTypeToken) {
    return clauseTypeToken.isModifies();
}

void ModifiesParser::checkArguments(list<ClauseArgument>& args) {
    //Modifies clause should have exactly 2 arguments
    assert(args.size() == 2);

    // first arg cannot be wildcard, variable or constant
    ClauseArgument arg = args.front();
    if (arg.isWildcard() || arg.isVariableSynonym() || arg.isConstantSynonym()) {
        this->semanticErrorMessage = "First arg for Uses cannot be wildcard, variable or constant";
    }

    arg = args.back();

    // syntactically, second arg must be an entRef which cannot be a number
    if (arg.isLineNumber()) {
        throw PQLSyntaxError("Second arg for Modifies cannot be a number");
    }

    // semantically, second arg must be a wildcard or a variable
    if (!arg.isWildcard() && !arg.isVariableSynonym() && !arg.isStringLiteral()) {
        this->semanticErrorMessage =
            "Second arg for Modifies must be a wildcard, a \"NAME\" or a declared variable synonym"; //NOLINT(whitespace/line_length)
    }
}

shared_ptr<RelationshipClause> ModifiesParser::createClause(
    PQLToken clauseTypeToken, list<ClauseArgument>& args) {
    //Modifies clause should have exactly 2 arguments
    assert(args.size() == 2);

    if (args.front().isStmtRefNoWildcard()) {
        return make_shared<ModifiesSClause>(args.front(), args.back());
    } else {
        return make_shared<ModifiesPClause>(args.front(), args.back());
    }
}
