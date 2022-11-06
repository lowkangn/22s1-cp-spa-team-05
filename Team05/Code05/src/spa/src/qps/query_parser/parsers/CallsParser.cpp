#include <qps/query_parser/parsers/CallsParser.h>

bool CallsParser::isCorrectClauseType(PQLToken clauseTypeToken) {
    if (tokens.size() == 1) {
        throw PQLSyntaxError("Query ended after 'Calls'");
    }

    PQLToken secondToken = *std::next(this->tokens.begin());
    if (secondToken.isStarModifier()) {
        this->isStar = true;
        this->tokens.pop_front();
    }

    return clauseTypeToken.isCalls();
}

void CallsParser::checkArguments(list<ClauseArgument>& args) {
    // Calls clause should have exactly 2 arguments
    assert(args.size() == 2);

    // semantically, both args must be either wildcards or procedures
    for (ClauseArgument arg : args) {
        if (arg.isLineNumber()) {
            throw PQLSyntaxError("Each arg for Calls/Calls* cannot be a number");
        }

        if (!arg.isWildcard() && !arg.isProcedureSynonym() && !arg.isStringLiteral()) {
            this->semanticErrorMessage =
                "Each arg for Calls/Calls* must be must be a wildcard, a \"NAME\" or a declared procedure synonym"; //NOLINT(whitespace/line_length)
        }
    }
}

shared_ptr<RelationshipClause> CallsParser::createClause(
    PQLToken clauseTypeToken, list<ClauseArgument>& args) {
    //Calls clause should have exactly 2 arguments
    assert(args.size() == 2);

    if (this->isStar) {
        return make_shared<CallsTClause>(args.front(), args.back());
    } else {
        return make_shared<CallsClause>(args.front(), args.back());
    }
}
