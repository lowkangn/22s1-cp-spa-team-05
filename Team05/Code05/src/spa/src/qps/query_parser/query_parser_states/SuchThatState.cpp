#include <qps/query_parser/query_parser_states/SuchThatState.h>
#include <qps/query_parser/QueryParser.h>

void SuchThatState::parseOneClause(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) {
    if (tokens.empty()) {
        throw PQLSyntaxError("Missing relRef after such that");
    }
    PQLToken token = tokens.front();
    shared_ptr<SuchThatClauseParser> parserPointer;

    if (token.isModifies()) {
        parserPointer = make_shared<ModifiesParser>(tokens, declarations);
    }
    else if (token.isParent()) {
        parserPointer = make_shared<ParentParser>(tokens, declarations);
    }
    else if (token.isUses()) {
        parserPointer = make_shared<UsesParser>(tokens, declarations);
    }
    else if (token.isFollows()) {
        parserPointer = make_shared<FollowsParser>(tokens, declarations);
    }
    else if (token.isCalls()) {
        parserPointer = make_shared<CallsParser>(tokens, declarations);
    }
    else if (token.isNext()) {
        parserPointer = make_shared<NextParser>(tokens, declarations);
    }
    else if (token.isAffects()) {
        parserPointer = make_shared<AffectsParser>(tokens, declarations);
    }
    else {
        throw PQLSyntaxError("Only Modifies, Uses, Parent/Parent*, Follows/Follows*, Calls/Calls*, Next/Next*, Affects/Affects* are supported as such that clauses.");
    }
    shared_ptr<RelationshipClause> clause = parserPointer->parse();
    this->context->addRelationshipClause(clause);
    this->context->setSemanticErrorFromParser(parserPointer);
}