#include <qps/query_parser/query_parser_states/SuchThatState.h>
#include <qps/query_parser/QueryParser.h>

void SuchThatState::parseOneClause(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) {
    if (tokens.empty()) {
        throw PQLSyntaxError("Missing relRef after such that");
    }
    PQLToken token = tokens.front();
    shared_ptr<SuchThatClauseParser> parserPointer;

    if (token.isModifies()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ModifiesParser(tokens, declarations));
    }
    else if (token.isParent()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ParentParser(tokens, declarations));
    }
    else if (token.isUses()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new UsesParser(tokens, declarations));
    }
    else if (token.isFollows()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new FollowsParser(tokens, declarations));
    }
    else if (token.isCalls()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new CallsParser(tokens, declarations));
    }
    else {
        throw PQLSyntaxError("Only Modifies, Uses, Parent/Parent*, Follows/Follows* are supported as such that clauses.");
    }
    shared_ptr<RelationshipClause> clause = parserPointer->parse();
    this->context->addRelationshipClause(clause);
    this->context->setSemanticErrorFromParser(parserPointer);
}