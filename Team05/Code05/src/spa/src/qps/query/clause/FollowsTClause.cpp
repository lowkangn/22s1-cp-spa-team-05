#include <qps/query/clause/FollowsTClause.h>

shared_ptr<RelationshipClauseResult> FollowsTClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::FOLLOWSSTAR, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool FollowsTClause::equals(shared_ptr <RelationshipClause> other) {
	if (dynamic_pointer_cast<FollowsTClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<FollowsTClause> otherFollowsT = dynamic_pointer_cast<FollowsTClause>(other);
	return (this->lhs == otherFollowsT->lhs) && (this->rhs == otherFollowsT->rhs);
}
