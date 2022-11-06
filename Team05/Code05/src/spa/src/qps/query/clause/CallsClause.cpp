#include <qps/query/clause/CallsClause.h>

shared_ptr<RelationshipClauseResult> CallsClause::execute(shared_ptr<PKBQueryHandler> pkb) {
    vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
        PKBTrackedRelationshipType::CALLS,
        this->lhs,
        this->rhs);
    return make_shared<RelationshipClauseResult>(this->lhs,
        this->rhs, relationships);
}

bool CallsClause::equals(shared_ptr<RelationshipClause> other) {
    if (dynamic_pointer_cast<CallsClause>(other) == nullptr) {
        return false;
    }
    shared_ptr<CallsClause> otherCalls = dynamic_pointer_cast<CallsClause>(other);
    return (this->lhs == otherCalls->lhs) && (this->rhs == otherCalls->rhs);
}
