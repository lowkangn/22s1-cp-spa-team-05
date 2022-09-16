#include <qps/query/clause/ModifiesSClause.h>

shared_ptr<RelationshipClauseResult> ModifiesSClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
			PKBTrackedRelationshipType::MODIFIES, lhs, rhs);
	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool ModifiesSClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<ModifiesSClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<ModifiesSClause> otherModifies = dynamic_pointer_cast<ModifiesSClause>(other);
	return (this->lhs == otherModifies->lhs) && (this->rhs == otherModifies->rhs);
}
