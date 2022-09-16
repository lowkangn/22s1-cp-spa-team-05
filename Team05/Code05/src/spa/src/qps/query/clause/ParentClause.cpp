#include <qps/query/clause/ParentClause.h>

shared_ptr<RelationshipClauseResult> ParentClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::PARENT, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool ParentClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<ParentClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<ParentClause> otherParent = dynamic_pointer_cast<ParentClause>(other);
	return (this->lhs == otherParent->lhs) && (this->rhs == otherParent->rhs);
}
