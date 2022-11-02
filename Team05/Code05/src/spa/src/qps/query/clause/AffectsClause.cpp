#include <qps/query/clause/AffectsClause.h>

#include <query_evaluator/CfgClauseOptimiser.h>

PKBTrackedRelationshipType AffectsClause::getPkbTrackedRelationshipType() {
    return PKBTrackedRelationshipType::AFFECTS;
}

void AffectsClause::acceptClauseOptimiser(CfgClauseOptimiser* optimiser) {
    optimiser->visitAffectsClause(this->lhs, this->rhs, shared_ptr<AffectsClause>(this));
}

bool AffectsClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<AffectsClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<AffectsClause> otherAffects = dynamic_pointer_cast<AffectsClause>(other);
	return (this->lhs == otherAffects->lhs) && (this->rhs == otherAffects->rhs);
}
