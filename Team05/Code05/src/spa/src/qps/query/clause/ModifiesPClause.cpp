#include <qps/query/clause/ModifiesPClause.h>

shared_ptr<RelationshipClauseResult> ModifiesPClause::execute(shared_ptr<PKB> pkb) {
	vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
			PKBTrackedRelationshipType::MODIFIES, lhs, rhs);
	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(lhs, rhs, relationships));
}

bool ModifiesPClause::equals(const RelationshipClause* other) {
	if (dynamic_cast<const ModifiesPClause*>(other) == nullptr) {
		return false;
	}
	ModifiesPClause otherModifiesP = *dynamic_cast<const ModifiesPClause*>(other);
	return this->lhs == otherModifiesP.lhs && this->rhs == otherModifiesP.rhs;
}