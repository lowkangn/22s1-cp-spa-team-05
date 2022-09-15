#include <qps/query/clause/ParentTClause.h>

shared_ptr<RelationshipClauseResult> ParentTClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::PARENTSTAR, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool ParentTClause::equals(const RelationshipClause* other) {
	if (dynamic_cast<const ParentTClause*>(other) == nullptr) {
		return false;
	}
	ParentTClause otherParentT = *dynamic_cast<const ParentTClause*>(other);
	return this->lhs == otherParentT.lhs && this->rhs == otherParentT.rhs;
}


