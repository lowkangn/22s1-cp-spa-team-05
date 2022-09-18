#include <qps/query/clause/FollowsClause.h>

shared_ptr<RelationshipClauseResult> FollowsClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::FOLLOWS, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool FollowsClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<FollowsClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<FollowsClause> otherFollows = dynamic_pointer_cast<FollowsClause>(other);
	return (this->lhs == otherFollows->lhs) && (this->rhs == otherFollows->rhs);
}
