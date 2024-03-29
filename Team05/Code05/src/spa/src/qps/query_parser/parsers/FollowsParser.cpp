#include <qps/query_parser/parsers/FollowsParser.h>

bool FollowsParser::isCorrectClauseType(PQLToken clauseTypeToken) {
    if (tokens.size() == 1) {
        throw PQLSyntaxError("Query ended after 'Follows'");
    }

    PQLToken secondToken = *std::next(this->tokens.begin());
    if (secondToken.isStarModifier()) {
        this->isStar = true;
        this->tokens.pop_front();
    }

    return clauseTypeToken.isFollows();
}

void FollowsParser::checkArguments(list<ClauseArgument>& args) {
    //Follows clause should have exactly 2 arguments
    assert(args.size() == 2);

    for (ClauseArgument arg : args) {
        if (arg.isStringLiteral()) {
            throw PQLSyntaxError(
                "Both args for Follows/Follows* must be stmtRefs, which cannot be a string");
        }

        if (!arg.isWildcard() && !arg.isStmtRefNoWildcard()) {
            this->semanticErrorMessage = "Both args for Follows/Follows* must be stmtRefs";
        }
    }
}

shared_ptr<RelationshipClause> FollowsParser::createClause(
    PQLToken clauseTypeToken, list<ClauseArgument>& args) {
    //Follows clause should have exactly 2 arguments
    assert(args.size() == 2);

    if (this->isStar) {
        return make_shared<FollowsTClause>(args.front(), args.back());
    } else {
        return make_shared<FollowsClause>(args.front(), args.back());
    }
}
