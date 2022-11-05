#include <qps/query/clause/AffectsTClause.h>
#include <query_evaluator/CfgClauseOptimiser.h>

PKBTrackedRelationshipType AffectsTClause::getPkbTrackedRelationshipType() {
    return PKBTrackedRelationshipType::AFFECTSSTAR;
}

void AffectsTClause ::acceptClauseOptimiser(CfgClauseOptimiser* optimiser) {
    optimiser->visitAffectsTClause(this->lhs, this->rhs, this);
}

bool AffectsTClause::isAlwaysEmpty() {
    for (const ClauseArgument& arg : { this->lhs, this->rhs }) {
        bool argCanAffect = arg.isStmtSynonym() || arg.isAssignSynonym()
            || arg.isWildcard() || arg.isLineNumber();
        if (!argCanAffect) {
            return true;
        }
    }
    return false;
}

bool AffectsTClause::equals(shared_ptr <RelationshipClause> other) {
    if (dynamic_pointer_cast<AffectsTClause>(other) == nullptr) {
        return false;
    }
    shared_ptr<AffectsTClause> otherAffectsT = dynamic_pointer_cast<AffectsTClause>(other);
    return (this->lhs == otherAffectsT->lhs) && (this->rhs == otherAffectsT->rhs);
}
