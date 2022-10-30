#include <qps/query/clause/NextTClause.h>

shared_ptr<RelationshipClauseResult> NextTClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::NEXTSTAR, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool NextTClause::equals(shared_ptr <RelationshipClause> other) {
	if (dynamic_pointer_cast<NextTClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<NextTClause> otherNextT = dynamic_pointer_cast<NextTClause>(other);
	return (this->lhs == otherNextT->lhs) && (this->rhs == otherNextT->rhs);
}
