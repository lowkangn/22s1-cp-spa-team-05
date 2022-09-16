#include <qps/query/clause/UsesPClause.h>

shared_ptr<RelationshipClauseResult> UsesPClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::USES, lhs, rhs);
	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool UsesPClause::equals(const RelationshipClause* other) {
	if (dynamic_cast<const UsesPClause*>(other) == nullptr) {
		return false;
	}
	UsesPClause otherUsesP = *dynamic_cast<const UsesPClause*>(other);
	return this->lhs == otherUsesP.lhs && this->rhs == otherUsesP.rhs;
}
