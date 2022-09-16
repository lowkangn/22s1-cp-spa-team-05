#include <qps/query/clause/ParentClause.h>

shared_ptr<RelationshipClauseResult> ParentClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::PARENT, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool ParentClause::equals(const RelationshipClause* other) {
	if (dynamic_cast<const ParentClause*>(other) == nullptr) {
		return false;
	}
	ParentClause otherParent = *dynamic_cast<const ParentClause*>(other);
	return this->lhs == otherParent.lhs && this->rhs == otherParent.rhs;
}
