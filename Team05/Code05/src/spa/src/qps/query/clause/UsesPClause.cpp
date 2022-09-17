#include <qps/query/clause/UsesPClause.h>

shared_ptr<RelationshipClauseResult> UsesPClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::USES, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool UsesPClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<UsesPClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<UsesPClause> otherUsesP = dynamic_pointer_cast<UsesPClause>(other);
	return (this->lhs == otherUsesP->lhs) && (this->rhs == otherUsesP->rhs);
}
