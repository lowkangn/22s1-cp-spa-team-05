#include <qps/query/clause/UsesSClause.h>

shared_ptr<RelationshipClauseResult> UsesSClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
		PKBTrackedRelationshipType::USES, lhs, rhs);
	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool UsesSClause::equals(const RelationshipClause* other) {
	if (dynamic_cast<const UsesSClause*>(other) == nullptr) {
		return false;
	}
	UsesSClause otherUses = *dynamic_cast<const UsesSClause*>(other);
	return this->lhs == otherUses.lhs && this->rhs == otherUses.rhs;
}
