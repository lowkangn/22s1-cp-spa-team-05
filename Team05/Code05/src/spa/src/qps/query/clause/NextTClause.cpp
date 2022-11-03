#include <qps/query/clause/NextTClause.h>
#include <qps/query_evaluator/CfgClauseOptimiser.h>

PKBTrackedRelationshipType NextTClause::getPkbTrackedRelationshipType() {
    return PKBTrackedRelationshipType::NEXTSTAR;
}

void NextTClause::acceptClauseOptimiser(CfgClauseOptimiser* optimiser) {
    optimiser->visitNextTClause(this->lhs, this->rhs, this);
}

bool NextTClause::equals(shared_ptr <RelationshipClause> other) {
	if (dynamic_pointer_cast<NextTClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<NextTClause> otherNextT = dynamic_pointer_cast<NextTClause>(other);
	return (this->lhs == otherNextT->lhs) && (this->rhs == otherNextT->rhs);
}
