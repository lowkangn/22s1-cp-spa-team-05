#include <qps/query_parser/parsers/ParentParser.h>

bool ParentParser::isCorrectClauseType(PQLToken clauseTypeToken) {
    if (tokens.size() == 1) {
        throw PQLSyntaxError("Query ended after 'Parent'");
    }

    PQLToken secondToken = *std::next(this->tokens.begin());
    if (secondToken.isStarModifier()) {
        this->isStar = true;
        this->tokens.pop_front();
    }

    return clauseTypeToken.isParent();
}

void ParentParser::checkArguments(list<ClauseArgument>& args) {
    //Parent clause should have exactly 2 arguments
    assert(args.size() == 2);

    for (ClauseArgument arg : args) {
        if (arg.isStringLiteral()) {
            throw PQLSyntaxError(
                "Both args for Parent/Parent* must be stmtRefs, which cannot be a string");
        }

        if (!arg.isWildcard() && !arg.isStmtRefNoWildcard()) {
            this->semanticErrorMessage = "Both args for Parent/Parent* must be stmtRefs";
        }
    }
}

shared_ptr<RelationshipClause> ParentParser::createClause(
    PQLToken clauseTypeToken, list<ClauseArgument>& args) {
    //Parent clause should have exactly 2 arguments
    assert(args.size() == 2);

    if (this->isStar) {
        return make_shared<ParentTClause>(args.front(), args.back());
    } else {
        return make_shared<ParentClause>(args.front(), args.back());
    }
}
