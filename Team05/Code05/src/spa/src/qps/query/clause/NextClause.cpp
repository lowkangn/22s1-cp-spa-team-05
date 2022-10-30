#include <qps/query/clause/NextClause.h>

shared_ptr<RelationshipClauseResult> NextClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::NEXT, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool NextClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<NextClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<NextClause> otherNext = dynamic_pointer_cast<NextClause>(other);
	return (this->lhs == otherNext->lhs) && (this->rhs == otherNext->rhs);
}
