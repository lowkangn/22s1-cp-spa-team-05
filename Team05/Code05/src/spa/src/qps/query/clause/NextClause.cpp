#include <qps/query/clause/NextClause.h>
#include <qps/query_evaluator/CfgClauseOptimiser.h>

PKBTrackedRelationshipType NextClause::getPkbTrackedRelationshipType() {
    return PKBTrackedRelationshipType::NEXT;
}

void NextClause::acceptClauseOptimiser(CfgClauseOptimiser* optimiser) {
    optimiser->visitNextClause(this->lhs, this->rhs, shared_ptr<NextClause>(this));
}

bool NextClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<NextClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<NextClause> otherNext = dynamic_pointer_cast<NextClause>(other);
	return (this->lhs == otherNext->lhs) && (this->rhs == otherNext->rhs);
}
