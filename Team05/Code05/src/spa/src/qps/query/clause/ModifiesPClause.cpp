#include <qps/query/clause/ModifiesPClause.h>

shared_ptr<RelationshipClauseResult> ModifiesPClause::execute(shared_ptr<PKBQueryHandler> pkb) {
    vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
        PKBTrackedRelationshipType::MODIFIES,
        lhs,
        rhs);
    return make_shared<RelationshipClauseResult>(this->lhs,
        this->rhs, relationships);
}

bool ModifiesPClause::equals(shared_ptr<RelationshipClause> other) {
    if (dynamic_pointer_cast<ModifiesPClause>(other) == nullptr) {
        return false;
    }
    shared_ptr<ModifiesPClause> otherModifiesP = dynamic_pointer_cast<ModifiesPClause>(other);
    return (this->lhs == otherModifiesP->lhs) && (this->rhs == otherModifiesP->rhs);
}
