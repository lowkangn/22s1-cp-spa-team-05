#include <qps/query/clause/UsesSClause.h>

shared_ptr<RelationshipClauseResult> UsesSClause::execute(shared_ptr<PKBQueryHandler> pkb) {
    vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
        PKBTrackedRelationshipType::USES,
        this->lhs,
        this->rhs);
    return make_shared<RelationshipClauseResult>(this->lhs, this->rhs, relationships);
}

bool UsesSClause::equals(shared_ptr<RelationshipClause> other) {
    if (dynamic_pointer_cast<UsesSClause>(other) == nullptr) {
        return false;
    }
    shared_ptr<UsesSClause> otherUses = dynamic_pointer_cast<UsesSClause>(other);
    return (this->lhs == otherUses->lhs) && (this->rhs == otherUses->rhs);
}
