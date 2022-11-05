#include <qps/query/clause/AffectsTClause.h>

shared_ptr<RelationshipClauseResult> AffectsTClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::AFFECTSSTAR, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool AffectsTClause::equals(shared_ptr <RelationshipClause> other) {
	if (dynamic_pointer_cast<AffectsTClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<AffectsTClause> otherAffectsT = dynamic_pointer_cast<AffectsTClause>(other);
	return (this->lhs == otherAffectsT->lhs) && (this->rhs == otherAffectsT->rhs);
}
