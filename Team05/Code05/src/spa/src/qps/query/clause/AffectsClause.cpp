#include <qps/query/clause/AffectsClause.h>

shared_ptr<RelationshipClauseResult> AffectsClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::AFFECTS, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool AffectsClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<AffectsClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<AffectsClause> otherAffects = dynamic_pointer_cast<AffectsClause>(other);
	return (this->lhs == otherAffects->lhs) && (this->rhs == otherAffects->rhs);
}
