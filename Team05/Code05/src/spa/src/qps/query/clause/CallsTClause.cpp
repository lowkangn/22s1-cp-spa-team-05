#include <qps/query/clause/CallsTClause.h>

shared_ptr<RelationshipClauseResult> CallsTClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::CALLSSTAR, this->lhs, this->rhs);
	return shared_ptr<RelationshipClauseResult>(
		new RelationshipClauseResult(this->lhs, this->rhs, relationships));
}

bool CallsTClause::equals(shared_ptr <RelationshipClause> other) {
	if (dynamic_pointer_cast<CallsTClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<CallsTClause> otherCallsT = dynamic_pointer_cast<CallsTClause>(other);
	return (this->lhs == otherCallsT->lhs) && (this->rhs == otherCallsT->rhs);
}
