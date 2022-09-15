#include <qps/query/clause/ModifiesSClause.h>

shared_ptr<RelationshipClauseResult> ModifiesSClause::execute(shared_ptr<PKB> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
			PKBTrackedRelationshipType::MODIFIES, lhs, rhs);
	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool ModifiesSClause::equals(const RelationshipClause* other) {
	if (dynamic_cast<const ModifiesSClause*>(other) == nullptr) {
		return false;
	}
	ModifiesSClause otherModifies = *dynamic_cast<const ModifiesSClause*>(other);
	return this->lhs == otherModifies.lhs && this->rhs == otherModifies.rhs;
}
