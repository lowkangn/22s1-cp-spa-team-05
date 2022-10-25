#include <qps/query/clause/PatternAssignClause.h>

PKBTrackedStatementType PatternAssignClause::getStatementType() {
	return PKBTrackedStatementType::ASSIGN;
}

bool PatternAssignClause::equals(shared_ptr<PatternClause> other) {
	if (dynamic_pointer_cast<PatternAssignClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<PatternAssignClause> otherPatternAssign = dynamic_pointer_cast<PatternAssignClause>(other);
	return (this->patternArg == otherPatternAssign->patternArg) && (this->lhs == otherPatternAssign->lhs) && (this->rhs == otherPatternAssign->rhs);
}
