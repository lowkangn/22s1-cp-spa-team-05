#include <qps/query/clause/AffectsClause.h>

#include <query_evaluator/CfgClauseOptimiser.h>

PKBTrackedRelationshipType AffectsClause::getPkbTrackedRelationshipType() {
    return PKBTrackedRelationshipType::AFFECTS;
}

void AffectsClause::acceptClauseOptimiser(CfgClauseOptimiser* optimiser) {
    optimiser->visitAffectsClause(this->lhs, this->rhs, this);
}

bool AffectsClause::isAlwaysEmpty() {
    for (const ClauseArgument& arg : {this->lhs, this->rhs}) {
        bool argCanAffect = arg.isStmtSynonym() || arg.isAssignSynonym()
            || arg.isWildcard() || arg.isLineNumber();
        if (!argCanAffect) {
            return true;
        }
    }
    return false;
}


bool AffectsClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<AffectsClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<AffectsClause> otherAffects = dynamic_pointer_cast<AffectsClause>(other);
	return (this->lhs == otherAffects->lhs) && (this->rhs == otherAffects->rhs);
}
