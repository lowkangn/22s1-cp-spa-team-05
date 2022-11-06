#include <qps/query/clause/ParentTClause.h>

shared_ptr<RelationshipClauseResult> ParentTClause::execute(shared_ptr<PKBQueryHandler> pkb) {
    vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
        PKBTrackedRelationshipType::PARENTSTAR,
        this->lhs,
        this->rhs);
    return make_shared<RelationshipClauseResult>(this->lhs,
        this->rhs, relationships);
}

bool ParentTClause::equals(shared_ptr<RelationshipClause> other) {
    if (dynamic_pointer_cast<ParentTClause>(other) == nullptr) {
        return false;
    }
    shared_ptr<ParentTClause> otherParentT = dynamic_pointer_cast<ParentTClause>(other);
    return (this->lhs == otherParentT->lhs) && (this->rhs == otherParentT->rhs);
}
